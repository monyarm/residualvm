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

#include "emixer.h"

EngineMixer::EngineMixer(int Width,int Height) : EngineBase(Width,Height){
	// Set up the desired audio profile
	SDL_AudioSpec desired;
	desired.freq=Cfg::Audio::Frequency;
	desired.samples=Cfg::Audio::Buffersize/2;
	desired.size=0;
	desired.callback=audio_callback;
	desired.userdata=this;
	desired.format=AUDIO_S16SYS;
	desired.channels=Cfg::Audio::Channels;
	thread_pointer=0;
	thread_lock=false;


	// Open the audio device
	if(!Cfg::Audio::Enabled){
		LogVerbose("Audio is disabled");
	}
	else if(SDL_OpenAudio(&desired,&hardware)<0){
		LogError("Couldn't open audio: %s",SDL_GetError());
		Cfg::Audio::Enabled=false;
	}
	else{
		// Give a verbose report
		LogVerbose("Opened audio device:");
		LogVerbose("\tSampling frequency\t\t\t%d Hz",(int)hardware.freq);
		LogVerbose("\tBuffer samples\t\t\t\t%d",(int)hardware.samples);
		LogVerbose("\tBuffer size\t\t\t\t%d B",(int)hardware.size);
		LogVerbose("\tAudio channels\t\t\t\t%d",(int)hardware.channels);
		LogVerbose("\tAudio format\t\t\t\t0x%x",(int)hardware.format);

		// Check result
		if(hardware.freq!=Cfg::Audio::Frequency){
			LogMessage("Overriding samplingrate: %d",hardware.freq);
			Cfg::Audio::Frequency=hardware.freq;
		}
		if(hardware.size!=(unsigned int)Cfg::Audio::Buffersize){
			LogMessage("Overriding framesize: %d",hardware.size);
			Cfg::Audio::Buffersize=hardware.size;
		}
		if(hardware.channels!=(unsigned int)Cfg::Audio::Channels){
			LogMessage("Overriding framesize: %d",hardware.channels);
			Cfg::Audio::Channels=hardware.channels;
		}

		// Prepare audio decoding thread
		thread_lock=true;
#ifndef VILE_BUILD_SDLHG
		thread_pointer=SDL_CreateThread(audio_thread,this);
#else
		thread_pointer=SDL_CreateThread(audio_thread,"audio",this);
#endif

		// Start audio decoding
		SDL_PauseAudio(0);
	}

#ifndef VILE_BUILD_SDLHG
	// Check for CDDA CDROM
	cdrom=0;
	if(Cfg::Audio::CDROM>=0){
		if(SDL_CDNumDrives()>Cfg::Audio::CDROM){
			cdrom=SDL_CDOpen(Cfg::Audio::CDROM);
		}
		if(cdrom==0){
			LogError("Invalid CDDA source: %d",Cfg::Audio::CDROM);
		}
		if(cdrom){
			if(CD_INDRIVE(SDL_CDStatus(cdrom))){
				LogVerbose("Registering CDDA source: %d",Cfg::Audio::CDROM);
			}
			else{
				LogError("No disc in CDROM %d",Cfg::Audio::CDROM);
				SDL_CDClose(cdrom);
				cdrom=0;
			}
		}
	}
#endif
}

EngineMixer::~EngineMixer(){
#ifndef VILE_BUILD_SDLHG
	if(cdrom){
		SDL_CDClose(cdrom);
	}
#endif
	if(Cfg::Audio::Enabled){
		LogVerbose("Closing down audio mixer");
		if(thread_lock){
			thread_lock=false;
			SDL_WaitThread(thread_pointer,NULL);
		}
		SDL_CloseAudio();
		StopMusic();
		StopSound(-1);
	}
}

EngineMixer::AudioChannel::AudioChannel(){
	mutex=SDL_CreateMutex();
	buffer=0;
	volume=1;
	enabled=true;
}

EngineMixer::AudioChannel::~AudioChannel(){
	SDL_DestroyMutex(mutex);
}

/*! \brief Stops current music playback
 */
void EngineMixer::StopMusic(){
	SDL_LockMutex(musicbuffer.mutex);
	if(musicbuffer.buffer){
		delete musicbuffer.buffer;
		musicbuffer.buffer=0;
	}
	SDL_UnlockMutex(musicbuffer.mutex);
}

/*! \brief Play CDDA track
 *  \param Track Track to play
 *  \return True if track could be played back
 *
 *  This method will play back a track using Cfg::Audio::CDROM. Negative 
 *  values effectively disables cdda, null selects the first physical cdrom 
 *  drive.
 */
bool EngineMixer::PlayCDDA(int Track){
	bool retval=false;
#ifndef VILE_BUILD_SDLHG
	if(cdrom && CD_INDRIVE(SDL_CDStatus(cdrom))){
		LogVerbose("Playing CDDA: %d",Track);
		SDL_CDPlayTracks(cdrom,Track,0,Track+1,0);
	}
#endif
	return retval;
}

/*! \brief Start playing a music score
 *  \param Music Musical score to play back
 *  \return True if track wachannel->s successfully loaded
 *
 *  Currently playing tracks are automatically faded out which will halt
 *  execution for some milliseconds whenever applicable.
 */
bool EngineMixer::PlayMusic(RWops *Music){
	// Try to load music resource
	bool retval=false;
	if(musicbuffer.enabled && Cfg::Audio::Enabled){
		StopMusic();
		if(Music && Music->Size()>8){
			// Try to identify header
			char header[8];
			Music->Seek(0,SEEK_SET);
			Music->Read(header,8);
			AudioBuffer *buffer=0;
			if(0){
			}
			else if(!strncmp(header,"MThd",4) ||
					!strncmp(header,"MIDI",4) ||
					!strncmp(header,"RMID",4)){
#ifdef VILE_FEATURE_FLUIDSYNTH
				buffer=new AudioFluid();
#else
				LogMessage("Recompile with fluidsynth for MIDI support");
#endif
			}
			else{
#ifdef VILE_FEATURE_FFMPEG
				// Turn to the swiss army knife ..
				buffer=new AudioFFMPEG();
#endif
			}

			// Try to open the resource
			Music->Seek(0,SEEK_SET);
			if(buffer && buffer->Open(Music)){
				SDL_LockMutex(musicbuffer.mutex);
				musicbuffer.buffer=buffer;
				musicbuffer.buffer->SetReplays(-1);
				musicbuffer.buffer->SetPlaying(true);
				SDL_UnlockMutex(musicbuffer.mutex);
				retval=true;
			}
			else if(buffer){
				delete buffer;
			}
		}
	}
	return retval;
}

/*! \brief Stops given audio channel
 *  \param Channel Channel to stop
 */
void EngineMixer::StopSound(int Channel){
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		if(soundbuffer[Channel].buffer){
			delete soundbuffer[Channel].buffer;
			soundbuffer[Channel].buffer=0;
		}
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
	}
	else if(Channel<0){
		for(int i=0;i<AUDIO_CHANNELS;i++){
			StopSound(i);
		}
	}
}

/*! \brief Plays sound in specified channel
 *  \param Sound Sound resource to decode and play
 *  \param Channel Audio channel
 *  \param Replays Number of times to play chunk
 *  \return True if the sound resource was started without problems
 */
bool EngineMixer::PlaySound(RWops *Sound,int Channel,int Replays){
	bool retval=false;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		if(soundbuffer[Channel].enabled && Cfg::Audio::Enabled){
#ifdef VILE_FEATURE_FFMPEG
			// Try to open the resource using ffmpeg
			StopSound(Channel);
			AudioChannel *channel=&soundbuffer[Channel];
			AudioBuffer *buffer=new AudioFFMPEG();
			if(buffer->Open(Sound)){
				channel->buffer=buffer;
				channel->buffer->SetPlaying(true);
				retval=true;
			}
			else{
				delete buffer;
				channel->buffer=0;
			}
#endif
		}
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
	}
	return retval;
}

/*! \brief Pauses background music
 */
void EngineMixer::PauseMusic(){
	SDL_LockMutex(musicbuffer.mutex);
	if(musicbuffer.buffer){
		musicbuffer.buffer->SetPlaying(false);
	}
	SDL_UnlockMutex(musicbuffer.mutex);
}

/*! \brief Resumes background music
 */
void EngineMixer::ResumeMusic(){
	SDL_LockMutex(musicbuffer.mutex);
	if(musicbuffer.enabled && musicbuffer.buffer){
		musicbuffer.buffer->SetPlaying(true);
	}
	SDL_UnlockMutex(musicbuffer.mutex);
}

/*! \brief Enables or disables background music
 *  \param Enabled True to enable
 */
void EngineMixer::SetMusicEnabled(bool Enabled){
	SDL_LockMutex(musicbuffer.mutex);
	musicbuffer.enabled=Enabled;
	SDL_UnlockMutex(musicbuffer.mutex);
}

/*! \brief Wether background music is enabled
 *  \return True if music is enabled
 *
 *  Background music will not load or decode at all unless it is enabled. All
 *  other bgm controls will be ignored if the music mixer is disabled.
 */
bool EngineMixer::GetMusicEnabled(){
	SDL_LockMutex(musicbuffer.mutex);
	bool retval=musicbuffer.enabled;
	SDL_UnlockMutex(musicbuffer.mutex);
	return retval;
}

/*! \brief Enable or disable a given audio channel
 *  \param Channel Audio channel to set
 *  \param Enabled True to enable channel
 *
 *  A disabled channel will refuse to load any audio resources, all other
 *  audio controls will ignore that channel as long as it is disabled.
 */
void EngineMixer::SetSoundEnabled(int Channel,bool Enabled){
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		soundbuffer[Channel].enabled=Enabled;
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
	}
}

/*! \brief Wether an audio channel is enabled
 *  \param Channel Audio channel to check
 *  \return True if channel is enabled
 */
bool EngineMixer::GetSoundEnabled(int Channel){
	bool retval=false;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		retval=soundbuffer[Channel].enabled;
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
	}
	return retval;
}

/*! \brief Set volume for background music
 *  \param Volume Integer from 0 to 100
 *  \return Always true
 */
bool EngineMixer::SetVolumeMusic(int Volume){
	SDL_LockMutex(musicbuffer.mutex);
	musicbuffer.volume=Volume/100.0;
	SDL_UnlockMutex(musicbuffer.mutex);
	return true;
}

/*! \brief Set volume for a given audio channel
 *  \param Channel Audio channel to set
 *  \param Volume Integer from 0 to 100
 *  \return True if channel was set
 */
bool EngineMixer::SetVolumeSound(int Channel,int Volume){
	bool retval=false;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		soundbuffer[Channel].volume=Volume/100.0;
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
		retval=true;
	}
	return retval;
}

/*! \brief Get volume for background music
 *  \return Integer from 0 to 100
 */
int EngineMixer::GetVolumeMusic(){
	int retval=0;
	SDL_LockMutex(musicbuffer.mutex);
	retval=musicbuffer.volume*100;
	SDL_UnlockMutex(musicbuffer.mutex);
	return retval;
}

/*! \brief Get volume for a given audio channel
 *  \param Channel Audio channel to set
 *  \return Integer from 0 to 100 (0 if channel was invalid)
 */
int EngineMixer::GetVolumeSound(int Channel){
	int retval=0;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		SDL_LockMutex(soundbuffer[Channel].mutex);
		retval=soundbuffer[Channel].volume*100;
		SDL_UnlockMutex(soundbuffer[Channel].mutex);
	}
	return retval;
}

/*! \brief Decodes and buffers audio data (Background thread)
 *  \param arg Engine object
 *  \return 0
 */
int EngineMixer::audio_thread(void *arg){
	EngineMixer *e=(EngineMixer*)arg;
	while(!e->thread_lock);
	LogVerbose("Audio decoding thread is ready");
	while(e->thread_lock){
		// Process music
		bool relax=true;
		SDL_LockMutex(e->musicbuffer.mutex);
		if(e->musicbuffer.enabled && e->musicbuffer.buffer){
			relax&=e->musicbuffer.buffer->Decode();
		}
		SDL_UnlockMutex(e->musicbuffer.mutex);

		// Process sound data
		for(int i=0;i<AUDIO_CHANNELS;i++){
			SDL_LockMutex(e->soundbuffer[i].mutex);
			if(e->soundbuffer[i].enabled && e->soundbuffer[i].buffer){
				relax&=e->soundbuffer[i].buffer->Decode();
			}
			SDL_UnlockMutex(e->soundbuffer[i].mutex);
		}

		// Relieve cpu
		if(relax){
			SDL_Delay(10);
		}
	}
	return 0;
}

/*! \brief Decodes and buffers audio data (Called internally from SDL)
 *  \param udata Engine object
 *  \param stream Buffer to fill with raw 16bit pcm
 *  \param len Length of stream
 */
void EngineMixer::audio_callback(void *udata,Uint8 *stream,int len){
	// Mix music data 
	memset(stream,0,len);
	EngineMixer *e=(EngineMixer*)udata;
	SDL_LockMutex(e->musicbuffer.mutex);
	if(e->musicbuffer.enabled && e->musicbuffer.buffer){
		e->musicbuffer.buffer->Mix(stream,len,e->musicbuffer.volume);
	}
	SDL_UnlockMutex(e->musicbuffer.mutex);

	// Mix sounds and voices
	for(int i=0;i<AUDIO_CHANNELS;i++){
		SDL_LockMutex(e->soundbuffer[i].mutex);
		if(e->soundbuffer[i].enabled && e->soundbuffer[i].buffer){
			e->soundbuffer[i].buffer->Mix(stream,len,e->soundbuffer[i].volume);
		}
		SDL_UnlockMutex(e->soundbuffer[i].mutex);
	}
}


