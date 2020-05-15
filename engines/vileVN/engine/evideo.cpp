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

#include "evideo.h"

EngineVideo::EngineVideo(int Width,int Height) : EngineMixer(Width,Height){
	asurface=0;
	aframe=0;
	videobuffer=0;
	thread_pointer=0;
	thread_lock=false;
	videomutex=SDL_CreateMutex();
	if(Cfg::Video::Overlay){
		//! \todo Find out why buffering video overlays causes memory corruption
		Cfg::Video::Framecount=1;
	}
	if(Cfg::Video::Enabled){
		thread_lock=true;
#ifdef VILE_BUILD_SDLHG
		thread_pointer=SDL_CreateThread(video_thread,"video",this);
#else
		thread_pointer=SDL_CreateThread(video_thread,this);
#endif
	}
	else{
		LogVerbose("Video is disabled");
	}
}

EngineVideo::~EngineVideo(){
	if(Cfg::Video::Enabled){
		LogVerbose("Closing down video player");
		StopVideo();
		if(thread_lock){
			thread_lock=false;
			SDL_WaitThread(thread_pointer,NULL);
		}
	}
	SDL_DestroyMutex(videomutex);
}

/*! \brief Background thread to decode and buffer video data
 */
int EngineVideo::video_thread(void *arg){
	EngineVideo *e=(EngineVideo*)arg;
	while(!e->thread_lock);
	LogVerbose("Video decoding thread is ready");
	bool decode;
	while(e->thread_lock){
		// Process music
		decode=false;
		SDL_LockMutex(e->videomutex);
		if(e->videobuffer){
			decode=e->videobuffer->Decode();
		}
		SDL_UnlockMutex(e->videomutex);

		// Relieve cpu
		if(!decode){
			SDL_Delay(10);
		}
	}
	return 0;
}

/*! \brief Event signaling start of video playback
 */
void EngineVideo::EventGameStartVideo(){
}

/*! \brief Event signaling end of video playback
 */
void EngineVideo::EventGameStopVideo(){
}

/*! \brief Stops video playback and frees all related objects
 */
void EngineVideo::StopVideo(){
	SDL_LockMutex(videomutex);
	if(asurface || videobuffer){
		EventGameStopVideo();
	}
	if(asurface){
		for(int i=0;asurface[i];i++){
			SDL_FreeSurface(asurface[i]);
		}
		delete asurface;
		asurface=0;
		aframe=0;
	}
	if(videobuffer){
		StopSound(0);
		delete videobuffer;
		videobuffer=0;
	}
	SDL_UnlockMutex(videomutex);
}

/*! \brief Plays an animation sequence
 *  \param Animation Nullterminated sequence to animate
 *  \return True if animation was accepted
 */
bool EngineVideo::PlayAnimation(SDL_Surface **Animation){
	StopVideo();
	if(Animation && Cfg::Video::Enabled){
		asurface=Animation;
		aframe=0;
		EventGameStartVideo();
	}
	return Animation;
}

/*! \brief Playback a video resource
 *  \param Video Video resource to decode and play
 *  \return True if Video was accepted
 */
bool EngineVideo::PlayVideo(RWops *Video){
	bool retval=false;
	if(Video && Cfg::Video::Enabled){
		StopVideo();
		SDL_LockMutex(videomutex);
#ifdef VILE_FEATURE_FFMPEG
		videobuffer=new VideoFFMPEG();
#endif
		if(videobuffer && videobuffer->Open(Video)){
			StopSound(-1);
			StopMusic();
			videofps=videobuffer->Framerate();
			LogVerbose("Opened video stream at %f fps",videofps);
			PlaySound(Video,0,0);
			retval=true;
		}
		else{
			delete videobuffer;
			videobuffer=0;
		}
		SDL_UnlockMutex(videomutex);
	}
	return retval;
}

/*! \brief Overrides paint event to display video
 *  \param Screen Display surface
 *  \return True if display was updates
 *
 *  This method will only act when video is loaded, otherwise calls will
 *  be passed down to the base implementation.
 */
bool EngineVideo::EventHostPaint(SDL_Surface *Screen){
	bool retval=false;
	if(videobuffer){
		// Blit video frames until we run out
		SDL_LockMutex(videomutex);
		bool running=videobuffer->Paint();
		SDL_UnlockMutex(videomutex);
		if(running){
			LimitFramerate(videofps);
		}
		else{
			StopVideo();
		}
	}
	else if(asurface){
		// Blit animation sequence until user cancels
		if(asurface[aframe]){
			SDL_BlitSurface(asurface[aframe++],0,Screen,0);
			retval=true;
		}
		else{
			aframe=0;
		}

		// Fixed framerate limit for animations
		LimitFramerate(10);
	}
	else{
		// Call base painter
		retval=EngineBase::EventHostPaint(Screen);
	}
	return retval;
}

/*! \brief Checks wether any video or animations are currently playing
 *  \return True if any video resources are playing
 */
bool EngineVideo::PlayingVideo(){
	return (asurface || videobuffer);
}

void EngineVideo::EventHostTick(){
	if(!PlayingVideo()){
		EngineBase::EventHostTick();
	}
}

void EngineVideo::EventHostMouseMove(SDL_Surface *Screen,int X,int Y){
	if(!PlayingVideo()){
		EngineBase::EventHostMouseMove(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseLeftDown(SDL_Surface *Screen,int X,int Y){
	if(!PlayingVideo()){
		EngineBase::EventHostMouseLeftDown(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseRightDown(SDL_Surface *Screen,int X,int Y){
	if(!PlayingVideo()){
		EngineBase::EventHostMouseRightDown(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseLeftUp(SDL_Surface *Screen,int X,int Y){
	if(PlayingVideo()){
		StopVideo();
	}
	else{
		EngineBase::EventHostMouseLeftUp(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseRightUp(SDL_Surface *Screen,int X,int Y){
	if(PlayingVideo()){
		StopVideo();
	}
	else{
		EngineBase::EventHostMouseRightUp(Screen,X,Y);
	}
}

void EngineVideo::EventHostKeyDown(SDLKey Key){
	if(PlayingVideo()){
		if(KEY_ACTION(Key)){
			StopVideo();
		}
	}
	else{
		EngineBase::EventHostKeyDown(Key);
	}
}

void EngineVideo::EventHostKeyUp(SDLKey Key){
	if(!PlayingVideo()){
		EngineBase::EventHostKeyUp(Key);
	}
}

