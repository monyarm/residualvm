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

#include "fade.h"

FadeBlack::FadeBlack(SDL_Rect Dst,Uint32 Duration) : Fade(Dst){
	SDL_Surface *Surface=EDL_CreateSurface(Dst.w,Dst.h);
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	boxRGBA(Surface,0,0,Dst.w,Dst.h,0,0,0,0xFF);
	SetFade(Surface,Rect,Duration);
	SDL_FreeSurface(Surface);
}

FadeWhite::FadeWhite(SDL_Rect Dst,Uint32 Duration) : Fade(Dst){
	SDL_Surface *Surface=EDL_CreateSurface(Dst.w,Dst.h);
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	boxRGBA(Surface,0,0,Dst.w,Dst.h,0xFF,0xFF,0xFF,0xFF);
	SetFade(Surface,Rect,Duration);
	SDL_FreeSurface(Surface);
}

FadeColor::FadeColor(SDL_Rect Dst,
		SDL_Color Color,Uint32 Duration) : Fade(Dst){
	SDL_Surface *Surface=EDL_CreateSurface(Dst.w,Dst.h);
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	boxRGBA(Surface,0,0,Dst.w,Dst.h,Color.r,Color.g,Color.b,0xFF);
	SetFade(Surface,Rect,Duration);
	SDL_FreeSurface(Surface);
}

FadeColor::FadeColor(SDL_Rect Dst,
		Uint8 R,Uint8 G,Uint8 B,Uint32 Duration) : Fade(Dst){
	SDL_Surface *Surface=EDL_CreateSurface(Dst.w,Dst.h);
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	boxRGBA(Surface,0,0,Dst.w,Dst.h,R,G,B,0xFF);
	SetFade(Surface,Rect,Duration);
	SDL_FreeSurface(Surface);
}

Fade::Fade(SDL_Rect Dst) : Animation(Dst) {
	// Set default values
	sfade=0;
	start=0;
	duration=0;
}

Fade::Fade(SDL_Rect Dst,SDL_Surface *Surface,
		SDL_Rect Rect,Uint32 Duration) : Animation(Dst) {
	// Set default values
	sfade=0;
	start=0;
	duration=0;

	// Prepare surface
	SetFade(Surface,Rect,Duration);
}

Fade::~Fade(){
	if(sfade){
		SDL_FreeSurface(sfade);
	}
}

void Fade::SetFade(SDL_Surface *Surface,SDL_Rect Rect,Uint32 Duration){
	// Prepare surface
	if(Surface && Duration){
		// Configure object for fading
		duration=Duration;
		start=0;

		// Find position relative to widget
		crect.x=pos.x;
		crect.y=pos.y;
		crect.w=Rect.w;
		crect.h=Rect.h;
		if(crect.w<pos.w){
			crect.w=pos.w;
		}
		if(crect.h<pos.h){
			crect.h=pos.h;
		}


		// Make an opaque copy of the new graphics
		if(sfade){
			SDL_FreeSurface(sfade);
		}
		sfade=EDL_CreateSurface(Rect.w,Rect.h);
		EDL_BlitSurface(Surface,&Rect,sfade,0);
	}

}

/*! \brief Completes the running fade effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the animation surface is deleted. Subsequent calls to Copy()
 *  will use the widget graphics as source.
 */
bool Fade::Skip(){
	bool retval=sfade;
	if(retval){
		SDL_SetAlpha(sfade,SDL_SRCALPHA,0xFF);
		Blit(sfade);
		SDL_FreeSurface(sfade);
		sfade=0;
	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Fade::Continue(){
	return sfade!=0;
}

/*! \brief Copies graphics
 *  \param Dst Target surface
 */
void Fade::Copy(SDL_Surface *Dst){
	// Assert time
	if(!start){
		Move(crect);
		start=SDL_GetTicks();
	}
	Uint32 now=SDL_GetTicks();
	if(sfade){
		if(now<start){
			// Overflow!!
			LogError("FADING:OVERRUN!");
			now=now+(0xFFFFFFFF-start);
			start=0;
		}
	}

	// Continue fade effect
	if(!sfade){
		// No fading in progress
		Widget::Copy(Dst);
	}
	else if(now>=start+duration){
		// Fade is complete ...
		EDL_SetAlpha(sfade,0,0xFF);
		Blit(sfade);
		SDL_FreeSurface(sfade);
		sfade=0;
		Widget::Copy(Dst);
	}
	else{
		// Calculate a new fade step based upon time
		double p=(now-start)/(double)duration;
		p=255-(EDL_HANNING(p,2)*255);
		//SDL_SetAlpha(sfade,SDL_SRCALPHA,(Uint8)EDL_LIMIT(p,0x00,0xFF));
		//EDL_BlitSurface(sfade,0,Dst,&crect);
		EDL_SetAlpha(sfade,0,(Uint8)EDL_LIMIT(p,0x00,0xFF));
		EDL_BlendSurface(sfade,0,Dst,&crect);
		Refresh();
	}
}


