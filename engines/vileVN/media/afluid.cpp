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

#include "afluid.h"

AudioFluid::AudioFluid(){
	datalock=SDL_CreateMutex();
	framelock=SDL_CreateMutex();
	midisize=0;
	midibuffer=0;
	settings=0;
	synth=0;
	player=0;

	// Reroute fluidsynth messages
	fluid_set_log_function(FLUID_PANIC,log,this);
	fluid_set_log_function(FLUID_ERR,log,this);
	fluid_set_log_function(FLUID_WARN,log,this);
	fluid_set_log_function(FLUID_INFO,log,this);
}

AudioFluid::~AudioFluid(){
	Close();
	SDL_DestroyMutex(datalock);
	SDL_DestroyMutex(framelock);
}

FLUID_FRAME::FLUID_FRAME(unsigned char *Buffer){
	buffer=Buffer;
}

FLUID_FRAME::~FLUID_FRAME(){
	delete [] buffer;
}

/*! \brief Passes Fluidsynth log messages back to ViLE
 *  \param Level Severity of the message
 *  \param Message Log message to dispatch
 *  \param Data Callback object (Not used)
 */
void AudioFluid::log(int Level,char *Message,void *Data){
	uString msg="FLUIDSYNTH: ";
	if(Level==FLUID_PANIC || Level==FLUID_ERR){
		LogError(msg+Message);
	}
	else{
		LogVerbose(msg+Message);
	}
}

/*! \brief Open a MIDI track using a RWops
 *  \param Resource The RWops resource to open
 *  \return True if resource was successfully opened
 */
bool AudioFluid::Open(RWops *Resource){
	// Drop existing
	bool retval=false;
	uString sf=Cfg::Audio::Soundfont;
	Close();
	SDL_LockMutex(datalock);
	if(!sf.length() || !EDL_ReadableFile(sf)){
		LogVerbose("No MIDI Soundfont");
	}
	else if(!Resource || Resource->Size()<16 || Resource->Size()>1024*1000){
		LogError("Invalid fluidsynth input data (%d bytes)",
				Resource?Resource->Size():0);
	}
	else if(!(settings=new_fluid_settings())){
		LogError("Failed to load fluidsynth settings");
	}
	else if(!(synth=new_fluid_synth(settings))){
		LogError("Failed to load fluidsynth synthesizer");
	}
	else if(!(player=new_fluid_player(synth))){
		LogError("Failed to load fluidsynth player");
	}
	else if((fluid_synth_sfload(synth,sf.c_str(),1)==FLUID_FAILED)){
		LogError("Failed to load fluidsynth font: %s",sf.c_str());
	}
	else{
		double freq=Cfg::Audio::Frequency;
		fluid_settings_setnum(settings,"synth.sample-rate",freq);
		//fluid_settings_setnum(settings,"synth.verbose",1);
		//fluid_settings_setnum(settings,"synth.gain",0.8);
		Resource->Seek(0,SEEK_SET);
		midisize=Resource->Size();
		midibuffer=new unsigned char[midisize];
		if(Resource->Read(midibuffer,midisize)<(int)midisize){
			LogError("Data was not readable");
		}
		else if(fluid_player_add_mem(player,midibuffer,midisize)!=FLUID_OK){
			LogError("Failed to open MIDI data");
		}
		else{
			LogVerbose("Opened %d bytes of fluidsynth data",midisize);
			fluid_player_play(player);
			retval=true;
		}
	}

	// Drop all objects if we failed
	if(!retval){
		if(player){
			fluid_player_stop(player);
			delete_fluid_player(player);
			player=0;
		}
		if(synth){
			delete_fluid_synth(synth);
			synth=0;
		}
		if(settings){
			delete_fluid_settings(settings);
			settings=0;
		}
		if(midibuffer){
			delete [] midibuffer;
			midibuffer=0;
			midisize=0;
		}
	}
	SDL_UnlockMutex(datalock);
	return retval;
}

/*! \brief Write synthesized data to framebuffer
 *  \return True if anything was decoded
 *
 *  This is called from the engine audio decoder thread, checks the current
 *  framebuffer and loads more data if appropriate. Does nothing unless a
 *  valid MIDI track is loaded.
 */ 
bool AudioFluid::Decode(){
	bool retval=false;
	SDL_LockMutex(datalock);
	if(player && !fluid_player_get_status(player)){
		if(replays==0){
			SDL_UnlockMutex(datalock);
			Close();
			SDL_LockMutex(datalock);
		}
		else{
			if(replays>0){
				replays--;
			}
			fluid_player_play(player);
		}
	}
	if(player){
		SDL_LockMutex(framelock);
		int c=framebuffer.Count();
		SDL_UnlockMutex(framelock);
		if(c<Cfg::Audio::Buffercount){
			unsigned char *buffer=new unsigned char[Cfg::Audio::Buffersize];
			if(fluid_synth_write_s16(synth,
						Cfg::Audio::Buffersize/4,
						(int16_t*)buffer,0,2,
						(int16_t*)buffer,1,2)==FLUID_OK){
				SDL_LockMutex(framelock);
				framebuffer.Queue(new FLUID_FRAME(buffer));
				SDL_UnlockMutex(framelock);
				retval=true;
			}
			else{
				LogError("Failed to synthesize MIDI");
			}
		}
	}
	SDL_UnlockMutex(datalock);
	return retval;
}

/*! \brief Mix synthesized MIDI to audio output
 *  \param Buffer Memory to fill with s16 audio data
 *  \param Length Size of Buffer
 *  \param Volume Volume to mix signal with
 */
void AudioFluid::Mix(unsigned char *Buffer,int Length,double Volume){
	SDL_LockMutex(framelock);
	FLUID_FRAME *frame=(FLUID_FRAME*)framebuffer.Pop();
	SDL_UnlockMutex(framelock);
	if(frame){
		int16_t *src=(int16_t*)frame->buffer;
		int16_t *dest=(int16_t*)Buffer;
		for(int i=0;i<Length/2;i++){
			int t=(*dest)+((*src)*Volume);
			int16_t d=(int16_t)t;
			if(t>0x7FFF)	d=0x7FFF;
			if(t<-0x7FFF)	d=0xFFFF;
			*dest=d;
			dest++;
			src++;
		}
		delete frame;
	}
}

/*! \brief Stops any audio and releases all resources
 */
void AudioFluid::Close(){
	SDL_LockMutex(datalock);
	if(player){
		fluid_player_stop(player);
		delete_fluid_player(player);
		player=0;
	}
	if(synth){
		delete_fluid_synth(synth);
		synth=0;
	}
	if(settings){
		delete_fluid_settings(settings);
		settings=0;
	}
	if(midibuffer){
		delete [] midibuffer;
		midibuffer=0;
		midisize=0;
	}
	SDL_UnlockMutex(datalock);
	SDL_LockMutex(framelock);
	FLUID_FRAME *frame;
	while((frame=(FLUID_FRAME*)framebuffer.Pop())){
		delete frame;
	}
	SDL_UnlockMutex(framelock);
}


