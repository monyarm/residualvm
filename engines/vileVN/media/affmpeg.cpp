/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "affmpeg.h"

AudioFFMPEG::AudioFFMPEG() : AudioBuffer() {
	// Initialize ffmpeg at first run
	static bool init=false;
	if(!init){
#if LIBAVCODEC_VERSION_MAJOR<54
		avcodec_init();
#endif
		av_log_set_level(AV_LOG_WARNING);
		av_log_set_callback(log_callback);
		init=true;
	}

	// Unset variables
	ops=0;
	pb=0;
	framecache=0;
	FormatContext=0;
	stream=0;
	index=-1;
	mintime=0;
	packetcache=0;
	packetsize=0;
	packettime=0;
	resampler=0;
	resamplebuffer=0;
	decoded_frame=0;

	// Create objects
	datalock=SDL_CreateMutex();
	framelock=SDL_CreateMutex();
}

AudioFFMPEG::~AudioFFMPEG(){
	Close();
	SDL_DestroyMutex(datalock);
	SDL_DestroyMutex(framelock);
	if(resamplebuffer){
		delete [] resamplebuffer;
	}
}

FFMPEG_AUDIO_FRAME::FFMPEG_AUDIO_FRAME(int Framesize,int64_t Time){
	buffer=(unsigned char*)av_malloc(Framesize);
	size=Framesize;
	pts=Time;
}

FFMPEG_AUDIO_FRAME::FFMPEG_AUDIO_FRAME(int Framesize){
	buffer=(unsigned char*)av_malloc(Framesize);
	size=Framesize;
	pts=AV_NOPTS_VALUE ;
}

FFMPEG_AUDIO_FRAME::~FFMPEG_AUDIO_FRAME(){
	av_free(buffer);
}

// Implement reading from blobs
int AudioFFMPEG::read_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Read(buf,size);
}

// Implement writing from blobs
int AudioFFMPEG::write_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Write(buf,size);
}

// Implement seeking from blobs
int64_t AudioFFMPEG::seek_callback(void *rwops,int64_t offset,int whence){
	RWops *ops=(RWops*)rwops;
	if(whence==AVSEEK_SIZE){
		return ops->Size();
	}
	else{
		return ops->Seek(offset,whence);
	}
}

// Passes ffmpeg log output to our verbose output
void AudioFFMPEG::log_callback(void *ptr,int level,const char* fmt,va_list vl){
	const int size=255;
	static char buffer1[size];
	static char buffer2[size];
	int cnt=vsnprintf(buffer1,size,fmt,vl);
	if(cnt>1){
		buffer1[size-1]=0;
		cnt=sprintf(buffer2,"FFMPEG: %s",buffer1);
		if(cnt>1){
			buffer2[cnt-1]=0;
			LogVerbose(buffer2);
		}
	}
}

/*! \brief Reads a single packet from the source data
 *  \return Read packet or NULL at error or eof
 */
AVPacket *AudioFFMPEG::GetPacket(){
	AVPacket *pack=(AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(pack);
	int read=av_read_frame(FormatContext,pack);
	if(read<0){
		// End of file
		av_free(pack);
		pack=0;
	}
	else{
		// Try to read packet
		if(av_dup_packet(pack)==0){
			if(pack->stream_index==index){
				return pack;
			}
		}

		// Read packet recursively
		av_free_packet(pack);
		pack=GetPacket();
	}
	return pack;
}

/*! \brief Reads and decodes one or more frames of audio
 *  \return True if any frames could be decoded
 */
bool AudioFFMPEG::GetFrame(){
	bool retval=false;
	SDL_LockMutex(datalock);
	if(FormatContext && stream && packetcache){
		// Get pack properties
		uint64_t time=av_rescale(packettime,
				stream->time_base.num,
				stream->time_base.den);
	    if(time!=AV_NOPTS_VALUE && time<mintime){
			stream->codec->skip_frame=AVDISCARD_ALL;
		}
		else{
			stream->codec->skip_frame=AVDISCARD_DEFAULT;
		}

#if LIBAVCODEC_VERSION_MAJOR>=54
		unsigned int psize=packetcache->size;
		while(packetcache->size>0){
			int got_frame=0;
			if(!decoded_frame){
				decoded_frame=avcodec_alloc_frame();
				if(!decoded_frame){
					LogError("Out of audio memory");
					break;
				}
				else{
					avcodec_get_frame_defaults(decoded_frame);
				}
			}
			int len=avcodec_decode_audio4(stream->codec,decoded_frame,
					&got_frame,packetcache);
			if(len<0){
				LogError("Audio decoder error");
				break;
			}
			if(got_frame){
				int asize=av_samples_get_buffer_size(NULL,
						stream->codec->channels,
						decoded_frame->nb_samples,
						stream->codec->sample_fmt,1);
				if(asize>0){
					retval|=Process(time,decoded_frame->data[0],asize);
				}
			}
			packetcache->size-=len;
			packetcache->data+=len;
		}
		packetcache->size+=psize;
		packetcache->data-=psize;
		av_free_packet(packetcache);
		packetcache=0;
#else
		// \deprecated Legacy avcodec interface
		if(packetsize>0){
			// Allocate aligned memory
			int asize=FRAMESIZE*2;
			DECLARE_ALIGNED(16,unsigned char,decodebuffer)[asize];
			memset(decodebuffer+AVCODEC_MAX_AUDIO_FRAME_SIZE,
					0,FF_INPUT_BUFFER_PADDING_SIZE);

			// Decode packet
#if LIBAVCODEC_VERSION_MAJOR>=52
			AVPacket tmppacket;
			av_init_packet(&tmppacket);
			tmppacket.data=packetcache->data+(packetcache->size-packetsize);
			tmppacket.size=packetsize;
			int len=avcodec_decode_audio3(stream->codec,
					(int16_t*)&decodebuffer,&asize,&tmppacket);
			av_free_packet(&tmppacket);
#else
			int len=avcodec_decode_audio2(stream->codec,
					(int16_t*)&decodebuffer,&asize,
					packetcache->data+(packetcache->size-packetsize),
					packetsize);
#endif

			// Verify results and update packetsize
			if(len<=0){
				LogError("Error decoding audio frame");
				stream->codec->skip_frame=AVDISCARD_ALL;
				av_free_packet(packetcache);
				packetcache=0;
				packetsize=0;
				asize=0;
			}
			else{
				packetsize-=len;
			}

			// Post-process and buffer up the decoded data
			if(asize && stream->codec->skip_frame!=AVDISCARD_ALL){
				retval|=Process(time,decodebuffer,asize);
			}
		}
		else{
			// Discard package
			av_free_packet(packetcache);
			packetcache=0;
			packetsize=0;
		}
#endif
	}

	// Check if we need a new packet
	if(FormatContext && !packetcache){
		if((packetcache=GetPacket())){
			packetsize=packetcache->size;
			if(packetcache->dts==(unsigned int)AV_NOPTS_VALUE){
				if(packettime<(unsigned int)stream->start_time){
					packettime=stream->start_time;
				}
				packettime+=1000*packetcache->duration;
			}
			else{
				packettime=(packetcache->dts-stream->start_time)*1000;
			}
		}
	}

	SDL_UnlockMutex(datalock);
	return retval;
}

/*! \brief Process decoded audio frames and move into audio queue
 *  \param Buffer Audio data
 *  \param Size Size of buffer
 */
bool AudioFFMPEG::Process(time_t Time,unsigned char *Buffer,unsigned int Size){
	bool retval=false;
	int outsize=Cfg::Audio::Buffersize;
	if(!framecache){
		framecache=new FFMPEG_AUDIO_FRAME(outsize,Time);
		framecache->size=0;
	}

	// Temporary (/Dirty) hack for mayclub
	if(stream->codec->sample_fmt==0){
		stream->codec->channels=1;
	}

	// Resample stream
	unsigned char *outbuffer=Buffer;
	if(resampler){
		if(!resamplebuffer){
			resamplebuffer=new Uint8[FRAMESIZE*2];
		}
		int outchannels=Cfg::Audio::Channels;
		int inchannels=stream->codec->channels;
		int written=audio_resample(resampler,
				(int16_t*)resamplebuffer,
				(int16_t*)Buffer,
				Size/(inchannels*2));

		if(written>0){
			outbuffer=resamplebuffer;
			Size=written*outchannels*2;
			if(Size>FRAMESIZE*2){
				LogError("Buffer overflow while resampling");
			}
		}
		else{
			LogError("Failed to convert bitrate");
		}
	}

	// Store frames as they are filled
	int aoffset=0;
	int space=outsize-framecache->size;
	int size=Size;
	while(space<=size){
		memcpy(framecache->buffer+framecache->size,
				outbuffer+aoffset,space);
		framecache->size=outsize;
		SDL_LockMutex(framelock);
		framebuffer.Queue(framecache);
		SDL_UnlockMutex(framelock);
		framecache=new FFMPEG_AUDIO_FRAME(outsize,Time);
		framecache->size=0;
		size-=space;
		aoffset+=space;
		space=outsize;
		retval=true;
	}

	// Store remaining data in framecache
	if(size>0){
		memcpy(framecache->buffer+framecache->size,
				outbuffer+aoffset,size);
		framecache->size+=size;
	}
	return retval;
}


/*! \brief Closes and flushes all currently opened data
 */
void AudioFFMPEG::CloseData(){
	SDL_LockMutex(datalock);
	if(decoded_frame){
		av_free(decoded_frame);
	}
	if(framecache){
		delete framecache;
		framecache=0;
	}
	if(packetcache){
		av_free_packet(packetcache);
		packetcache=0;
	}
	if(stream){
		avcodec_close(stream->codec);
		stream=0;
	}
	if(resampler){
		audio_resample_close(resampler);
		resampler=0;
	}
	if(FormatContext){
		// TODO: Loop through and close stream objects?
		//avcodec_close(FormatContext->streams[i]);
#if LIBAVCODEC_VERSION_MAJOR>=54
		avformat_close_input(&FormatContext);
#else
		av_close_input_stream(FormatContext);
		FormatContext=0;
#endif
	}
	if(pb){
		unsigned char *b=pb->buffer;
		pb->buffer=0;
		// This is seriously fucked across versions of the upstream library.
		// The IO stream might be closed automatically, but not necessarily
		// and the dedicated close function segfaults.
		//avio_close(pb);
		free(pb);
		free(b);
		pb=0;
	}
	if(ops){
		delete ops;
		ops=0;
	}
	stream=0;
	index=-1;
	mintime=0;
	decoded_frame=0;
	SDL_UnlockMutex(datalock);
}

/*! \brief Closes and flushes all currently opened data
 */
void AudioFFMPEG::CloseFrames(){
	SDL_LockMutex(framelock);
	FFMPEG_AUDIO_FRAME *frame;
	while((frame=(FFMPEG_AUDIO_FRAME*)framebuffer.Pop())){
		delete frame;
	}
	SDL_UnlockMutex(framelock);
}

/*! \brief Closes and flushes all currently opened data
 */
void AudioFFMPEG::Close(){
	CloseData();
	CloseFrames();
}

bool AudioFFMPEG::Seek(uint64_t Offset){
	bool retval=false;
	SDL_LockMutex(datalock);
	if(FormatContext){
		av_seek_frame(FormatContext,-1,0,AVSEEK_FLAG_BACKWARD);
		mintime=0;
		retval=true;
	}
	SDL_UnlockMutex(datalock);
	return retval;
}

/*! \brief Asserts that a sufficent number of frames are cached and ready
 *  \return True if anything was decoded
 *
 *  This method must be called regularly, preferably from a traditional
 *  decoding thread. This method returning false means that the calling
 *  thread can relax for a while as the stream is emty or the buffer is
 *  full.
 */
bool AudioFFMPEG::Decode(){
	// Get current framebuffer count
	bool retval=false;
	SDL_LockMutex(framelock);
	int c=framebuffer.Count();
	SDL_UnlockMutex(framelock);
	if(c<Cfg::Audio::Buffercount && FormatContext){
		if(!(retval=GetFrame())){
			// Handle end-of-file
			if(replays==0){
				CloseData();
			}
			else{
				if(replays>0){
					replays--;
				}
				Seek(0);
			}
		}
	}
	return retval;
}

/*! \brief Mixes decoded data to the output audio stream
 *
 *  This method should typically be called from within the applications audio
 *  callback. Cached frames should always be cached with the same bitrate and
 *  characteristics as the audio output, so the data can be copied directly
 *  without any further conversion.
 */
void AudioFFMPEG::Mix(unsigned char *Output,int Length,double Volume){
	SDL_LockMutex(framelock);
	FFMPEG_AUDIO_FRAME *frame=(FFMPEG_AUDIO_FRAME*)framebuffer.Pop();
	SDL_UnlockMutex(framelock);
	if(frame){
		// Mix audio data to output
		int16_t *src=(int16_t*)frame->buffer;
		int16_t *dest=(int16_t*)Output;
		for(int i=0;i<Length/2;i++){
			// Add sample to audio output
			int t=(*dest)+((*src)*Volume);
			int16_t d=(int16_t)t;
			if(t>0x7FFF)	d=0x7FFF;
			if(t<-0x7FFF)	d=0xFFFF;
			*dest=d;
			dest++;
			src++;
		}

		// Release frame
		delete frame;
	}
}

/*! \brief Opens and identifies an audio resouce from a RWops source
 *  \param Resource Source data
 *  \return True if Resource was identified as a valid audio resource
 */
bool AudioFFMPEG::Open(RWops *Resource){
	// Close existing and copy input data
	Close();
	SDL_LockMutex(datalock);
	ops=new RWops();
	if(!ops->OpenRW(Resource,false)){
		LogError("Failed to copy blob");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
	}

	// Create a custom stream io handler for the resource
	unsigned int iosize=2048+FF_INPUT_BUFFER_PADDING_SIZE;
	unsigned char *iobuffer=(unsigned char*)malloc(iosize);
	memset(iobuffer,0,iosize);
#if LIBAVCODEC_VERSION_MAJOR>=54
	pb=avio_alloc_context(iobuffer,iosize,0,ops,
			read_callback,write_callback,seek_callback);
#else
	pb=(ByteIOContext*)malloc(sizeof(ByteIOContext));
	memset(pb,0,sizeof(ByteIOContext));
	init_put_byte(pb,iobuffer,iosize,0,ops,
			read_callback,write_callback,seek_callback);
#endif

	// Probe input format manually
	AVProbeData probe_data;
	probe_data.filename="blob";
	int probesize=(1024*4)+AVPROBE_PADDING_SIZE;
	if(ops->Size()<probesize){
		probesize=ops->Size()+AVPROBE_PADDING_SIZE;
	}
	probe_data.buf_size=probesize-4;
	probe_data.buf=(uint8_t*)malloc(probesize);
	memset(probe_data.buf,0,probesize);
	ops->Read(probe_data.buf,probesize);
	ops->Seek(0,SEEK_SET);
	AVInputFormat *fmt=av_probe_input_format(&probe_data,1);
	free(probe_data.buf);
	probe_data.buf=0;
	if (!fmt){
        LogVerbose("FFMPEG could not identify unknown media"); 
		SDL_UnlockMutex(datalock);
		Close();
		return false;
	}
	fmt->flags|=AVFMT_NOFILE;
	// TODO: Free fmt at some point?

#if LIBAVCODEC_VERSION_MAJOR>=54
	if(!FormatContext){
		FormatContext=avformat_alloc_context();
	}
	FormatContext->pb=pb;
	if(avformat_open_input(&FormatContext,"RWops",fmt,0)!=0){
#else
	if(av_open_input_stream(&FormatContext,pb,"RWops",fmt,0)!=0){
#endif
        LogError("FFMPEG failed to open audio stream");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
    }

    // retrieve format information
#if LIBAVCODEC_VERSION_MAJOR>=54
    if(avformat_find_stream_info(FormatContext,NULL)<0){
#elif LIBAVCODEC_VERSION_MAJOR>=52
    if(av_find_stream_info(FormatContext)<0){
#else
	if(avformat_find_stream_info(FormatContext,NULL)<0){
#endif
        LogError("FFMPEG could not retrieve file info for stream");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
    }

	// Select first audio stream, discard all others
	bool retval=false;
    for(unsigned int i=0;!retval && i<FormatContext->nb_streams;i++){
        FormatContext->streams[i]->discard=AVDISCARD_ALL;
#if LIBAVCODEC_VERSION_MAJOR>=53
		enum AVMediaType type=FormatContext->streams[i]->codec->codec_type;
		if(type==AVMEDIA_TYPE_AUDIO){
#else
		enum CodecType type=FormatContext->streams[i]->codec->codec_type;
		if(type==CODEC_TYPE_AUDIO){
#endif
			// Try to find a decoder for this stream
			enum CodecID id=FormatContext->streams[i]->codec->codec_id;
			AVCodec *codec=avcodec_find_decoder(id);
			if(!codec){
				LogError("Could not find audio codec");
			}
#if LIBAVCODEC_VERSION_MAJOR>53
			else if(avcodec_open2(FormatContext->streams[i]->codec,codec,NULL)<0)
#else
			else if(avcodec_open(FormatContext->streams[i]->codec,codec)<0)
#endif
			{
				LogError("Failed to open audio codec");
			}
			else{
				// Cache stream properties
				stream=FormatContext->streams[i];
        		stream->discard=AVDISCARD_DEFAULT;
				bool resample=false;
				resample|=(stream->codec->sample_rate!=Cfg::Audio::Frequency);
				resample|=(stream->codec->channels!=Cfg::Audio::Channels);
				resample|=(stream->codec->sample_fmt!=AV_SAMPLE_FMT_S16);
				if(resample){
					resampler=av_audio_resample_init(
							Cfg::Audio::Channels,
							stream->codec->channels,
							(int)Cfg::Audio::Frequency,
							(int)stream->codec->sample_rate,
							AV_SAMPLE_FMT_S16,
							stream->codec->sample_fmt,
							16,10,0,1.0);
					if(!resampler){
						LogError("Failed to load ffmpeg resampling context");
					}
				}
				LogVerbose("Opened ffmpeg audio (%d channels,%d Hz,%d bps)",
						stream->codec->channels,
						stream->codec->sample_rate,
						stream->codec->bit_rate);
				retval=true;
				index=i;
			}
		}
	}

	// Check if any streams were found
	SDL_UnlockMutex(datalock);
	if(!retval){
		Close();
	}
    return retval;
}

