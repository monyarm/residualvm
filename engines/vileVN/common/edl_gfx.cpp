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

#include "edl_gfx.h"

inline void EDL_GetPixel_8bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	*red=surface->format->palette->colors[*pixel].r;
	*green=surface->format->palette->colors[*pixel].g;
	*blue=surface->format->palette->colors[*pixel].b;
	*alpha=0xFF;
}

inline void EDL_SetPixel_8bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	*pixel=SDL_MapRGB(surface->format,red,green,blue);
}

inline void EDL_GetPixel_16bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	Uint32 dc=*pixel;
	*red=(dc&surface->format->Rmask)>>surface->format->Rshift;
	*green=(dc&surface->format->Gmask)>>surface->format->Gshift;
	*blue=(dc&surface->format->Bmask)>>surface->format->Bshift;
	if(surface->format->Amask){
		*alpha=(dc&surface->format->Amask)>>surface->format->Ashift;
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_16bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint32 color=0;
	color|=(red<<surface->format->Rshift)&surface->format->Rmask;
	color|=(green<<surface->format->Gshift)&surface->format->Gmask;
	color|=(blue<<surface->format->Bshift)&surface->format->Bmask;
	color|=(alpha<<surface->format->Ashift)&surface->format->Amask;
	*((Uint16*)surface->pixels+y*surface->pitch/2+x)=color;
}

inline void EDL_GetPixel_24bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pix=(Uint8*)surface->pixels+y*surface->pitch+x*3;
	Uint8 rshift8=surface->format->Rshift/8;
	Uint8 gshift8=surface->format->Gshift/8;
	Uint8 bshift8=surface->format->Bshift/8;
	Uint8 ashift8=surface->format->Ashift/8;
	*red=*((pix)+rshift8);
	*green=*((pix)+gshift8);
	*blue=*((pix)+bshift8);
	if(surface->format->Amask){
		*alpha=*((pix)+ashift8);
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_24bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint8 *pix=(Uint8*)surface->pixels+y*surface->pitch+x*3;
	Uint32 color=blue<<16|green<<8|red;
	Uint8 rshift8=surface->format->Rshift/8;
	Uint8 gshift8=surface->format->Gshift/8;
	Uint8 bshift8=surface->format->Bshift/8;
	Uint8 ashift8=surface->format->Ashift/8;
	*(pix + rshift8) = color >> surface->format->Rshift;
	*(pix + gshift8) = color >> surface->format->Gshift;
	*(pix + bshift8) = color >> surface->format->Bshift;
	if(surface->format->Amask){
		*(pix + ashift8) = color >> surface->format->Ashift;
	}
}

inline void EDL_GetPixel_32bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint32 *pixel=(Uint32*)surface->pixels+y*surface->pitch/4+x;
	*red=((*pixel)&surface->format->Rmask)>>surface->format->Rshift;
	*green=((*pixel)&surface->format->Gmask)>>surface->format->Gshift;
	*blue=((*pixel)&surface->format->Bmask)>>surface->format->Bshift;
	if(surface->format->Amask){
		*alpha=((*pixel)&surface->format->Amask)>>surface->format->Ashift;
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_32bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint32 *pixel=(Uint32*)surface->pixels+y*surface->pitch/4+x;
	Uint32 R=(red<<surface->format->Rshift)&surface->format->Rmask;
	Uint32 G=(green<<surface->format->Gshift)&surface->format->Gmask;
	Uint32 B=(blue<<surface->format->Bshift)&surface->format->Bmask;
	Uint32 A=(alpha<<surface->format->Ashift)&surface->format->Amask;
	*pixel=R|G|B|A;
}

inline void EDL_GetPixel_auto(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	switch (surface->format->BytesPerPixel) {
		case 1:	EDL_GetPixel_8bit(surface,x,y,red,green,blue,alpha);	break;
		case 2:	EDL_GetPixel_16bit(surface,x,y,red,green,blue,alpha);	break;
		case 3:	EDL_GetPixel_24bit(surface,x,y,red,green,blue,alpha);	break;
		case 4:	EDL_GetPixel_32bit(surface,x,y,red,green,blue,alpha);	break;
	}
}

inline void EDL_SetPixel_auto(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	switch (surface->format->BytesPerPixel) {
		case 1:	EDL_SetPixel_8bit(surface,x,y,red,green,blue,alpha);	break;
		case 2:	EDL_SetPixel_16bit(surface,x,y,red,green,blue,alpha);	break;
		case 3:	EDL_SetPixel_24bit(surface,x,y,red,green,blue,alpha);	break;
		case 4:	EDL_SetPixel_32bit(surface,x,y,red,green,blue,alpha);	break;
	}
}

/*! \brief Reads and maps a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \param Red Pointer to receive red color value
 *  \param Green Pointer to receive green color value
 *  \param Blue Pointer to receive blue color value
 *  \param Alpha Pointer to receive alpha channel value
 */
void EDL_GetPixel(SDL_Surface *Surface,
		Sint16 X,Sint16 Y,
		Uint8 *Red,Uint8 *Green,Uint8 *Blue,Uint8 *Alpha){
	// Lock source
	if(SDL_MUSTLOCK(Surface)){
		if(SDL_LockSurface(Surface)<0){
			return;
		}
	}

	// Read pixel
	EDL_GetPixel_auto(Surface,X,Y,Red,Green,Blue,Alpha);

	// Unlock
	if(SDL_MUSTLOCK(Surface)){
		SDL_UnlockSurface(Surface);
	}
}

/*! \brief Reads and maps rgba value of a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \return Colorvalue of given pixel (0 if failed)
 */
Uint32 EDL_GetPixel(SDL_Surface *surface,Sint16 x,Sint16 y){
	Uint32 retval=0;
	if(surface){
		Uint8 r,g,b,a;
		EDL_GetPixel(surface,x,y,&r,&g,&b,&a);
		retval=(r<<24)|(g<<16)|(b<<8)|a;
	}
	return retval;
}

/*! \brief Reads and maps a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \param Red Pointer to receive red color value
 *  \param Green Pointer to receive green color value
 *  \param Blue Pointer to receive blue color value
 *  \param Alpha Pointer to receive alpha channel value
 */
void EDL_SetPixel(SDL_Surface *Surface,
		Sint16 X,Sint16 Y,
		Uint8 Red,Uint8 Green,Uint8 Blue,Uint8 Alpha){
	// Lock source
	if(SDL_MUSTLOCK(Surface)){
		if(SDL_LockSurface(Surface)<0){
			return;
		}
	}

	// Read pixel
	EDL_SetPixel_auto(Surface,X,Y,Red,Green,Blue,Alpha);

	// Unlock
	if(SDL_MUSTLOCK(Surface)){
		SDL_UnlockSurface(Surface);
	}
}

/*! \brief Saves a surface to specified file
 *  \param surface Surface to save
 *  \param Filename Filename to save as (A three digit number will be appended)
 */
void EDL_SaveSurface(SDL_Surface *surface,uString File){
	bool done=false;
	int cnt=0;
	while(!done){
		char fname[255];
		sprintf(fname,"%s-%03d.bmp",File.c_str(),cnt++);
		FILE *tf=fopen(fname,"rb");
		if(!tf){
			tf=fopen(fname,"wb");
			if(tf){
				done=true;
				fclose(tf);
				SDL_SaveBMP(surface,fname);
			}
		}
		else{
			fclose(tf);
		}
	}
}

/*! \brief Creates a new surface with alpha channel
 *  \param Width Width of new surface
 *  \param Height Height of new surface
 *  \returns New surface
 */
SDL_Surface *EDL_CreateSurface(int Width,int Height){
	if(Width>0 && Height>0){
	    return SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,Width,Height,32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
#else
				0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
#endif
	}
	else{
		return 0;
	}
}

/*! \brief Generates and blits a text surface (Including alpha layer)
 *  \param Text String to render
 *  \param Color Color to render text in
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  Text size will be automatically fitted for the destination rectangle
 */
void EDL_BlitText(uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect){
	SDL_Rect drect={0,0,dst->w,dst->h};
	if(dstrect){
		drect=*dstrect;
	}
	SDL_Surface *tmp=EDL_CreateText(Text,Color,drect.w,drect.h);
	if(tmp){
		EDL_BlitSurface(tmp,0,dst,&drect);
		SDL_FreeSurface(tmp);
	}
}

/*! \brief Renders a text string and blends it into a surface
 *  \param Text String to render
 *  \param Color Color to render text in
 *  \param dst Destination surface
 *  \param dstrect Destination surface
 *
 *  Text size will be automatically fitted for the destination rectangle
 */
void EDL_BlendText(uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect){
	SDL_Rect drect={0,0,dst->w,dst->h};
	if(dstrect){
		drect=*dstrect;
	}
	SDL_Surface *tmp=EDL_CreateText(Text,Color,drect.w,drect.h);
	if(tmp){
		EDL_BlendSurface(tmp,0,dst,&drect);
		SDL_FreeSurface(tmp);
	}
}

/*! \brief Calculates size of a given text
 *  \param Text String to calculate
 *  \param Size Font size (more or less the same as the height)
 *  \param Width Pointer to width variable
 *  \param Height Pointer to height variable
 *  \returns True if string can be rendered
 */
bool EDL_SizeText(uString Text,int Size,int *Width,int *Height){
	bool retval=false;
	if(Text.length()){
		uString face=Cfg::Font::default_face;
		TTF_Font *font=TTF_OpenFont(face.c_str(),Size);
		if(font){
			if(!TTF_SizeText(font,Text.c_str(),Width,Height)){
				retval=true;
			}
			TTF_CloseFont(font);
		}
	}
	return retval;
}

/*! \brief Creates a surface with a text that spans multiple lines
 *  \param Text String to render (Newlines indicates linebreak)
 *  \param Color Font color
 *  \param Width Width of the resulting surface
 *  \param Height Height of the resulting surface
 *  \return Surface with rendered text
 */
SDL_Surface *EDL_CreateMultilineText(uString Text,
		Uint32 Color,int Width,int Height){
	SDL_Surface *retval=EDL_CreateSurface(Width,Height);
	unsigned int h=Cfg::Font::default_size;
	unsigned int s=0;
	unsigned int e=0;
	unsigned int i=0;
	for(;e<Text.length();e++){
		if(Text[e]=='\n'){
			if(e>s){
				uString t=Text.substr(s,e-s);
				SDL_Surface *tmp=EDL_CreateText(t,Color,Width,h);
				SDL_Rect tr={0,h*i++,Width,h};
				EDL_BlitSurface(tmp,0,retval,&tr);
				SDL_FreeSurface(tmp);
			}
			s=e+1;
		}
	}
	if(e>s){
		uString t=Text.substr(s,e-s);
		SDL_Surface *tmp=EDL_CreateText(t,Color,Width,h);
		SDL_Rect tr={0,h*i++,Width,h};
		EDL_BlitSurface(tmp,0,retval,&tr);
		SDL_FreeSurface(tmp);
	}
	return retval;
}

/*! \brief Creates a blendable text and automatically sizes it
 *  \param Text Text to render
 *  \param Color Color to render the text in
 *  \param Width Width of result surface
 *  \param Height Height of result surface
 *  \return blendable surface with text rendered on it
 */
SDL_Surface *EDL_CreateText(uString Text,
		Uint32 Color,int Width,int Height){
	// Create and configure a font object
	SDL_Surface *retval=0;
	uString face=Cfg::Font::default_face;
	int size=Cfg::Font::default_size;
	FILE *tf=fopen(face.c_str(),"rb");
	if(!tf){
		LogError("Failed to find font: %s",face.c_str());
	}
	else{
		fclose(tf);
	}
	if(tf && size>0 && Text.length()){
		// Load font and poll for size
		int tw,th;
		TTF_Font *font=0;
		for(int i=4;i<Height;i++){
			font=TTF_OpenFont(face.c_str(),i);
			if(!TTF_SizeText(font,Text.c_str(),&tw,&th)){
				if(tw>Width || th>Height){
					TTF_CloseFont(font);
					break;
				}
				else{
					size=i;
				}
			}
			TTF_CloseFont(font);
		}
		font=TTF_OpenFont(face.c_str(),size);
		if(font){
			SDL_Color color={(Color>>24)&0xFF,(Color>>16)&0xFF,Color>>8};
			retval=EDL_RenderText(font,Text,color);
			TTF_CloseFont(font);
		}
	}
	return retval;
}

SDL_Surface *EDL_RenderText(TTF_Font *Font,const uString &Text,SDL_Color fg){
#if VILE_BUILD_UNICODE
	return TTF_RenderUTF8_Blended(Font,Text.c_str(),fg);
#else
	return TTF_RenderText_Blended(Font,Text.c_str(),fg);
#endif
}

/*! \brief Alphablends a bitmap onto a new surface using a colorkey
 *  \param Bitmap Source surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 *
 *  This lumpy thing autodetects a colorkey from the four corners.
 */
SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,SDL_Rect *Rect){
	// Autodetect colorkey
	Uint32 c1=EDL_GetPixel(Bitmap,0,0);
	Uint32 c2=EDL_GetPixel(Bitmap,Bitmap->w-1,0);
	Uint32 c3=EDL_GetPixel(Bitmap,0,Bitmap->h-1);
	Uint32 c4=EDL_GetPixel(Bitmap,Bitmap->w-1,Bitmap->h-1);
	int m1=0;
	int m2=0;
	int m3=0;
	int m4=0;
	if(c1==c2){
		m1++;
		m2++;
	}
	if(c1==c3){
		m1++;
		m3++;
	}
	if(c1==c4){
		m1++;
		m4++;
	}
	if(c2==c3){
		m2++;
		m3++;
	}
	if(c2==c4){
		m2++;
		m4++;
	}
	if(c3==c4){
		m2++;
		m3++;
	}

	if(m1>2){
		return EDL_ColorkeySurface(Bitmap,c1,Rect);
	}
	if(m2>2){
		return EDL_ColorkeySurface(Bitmap,c2,Rect);
	}
	if(m3>2){
		return EDL_ColorkeySurface(Bitmap,c3,Rect);
	}
	if(m4>2){
		return EDL_ColorkeySurface(Bitmap,c4,Rect);
	}
	if(m1>1){
		return EDL_ColorkeySurface(Bitmap,c1,Rect);
	}
	if(m2>1){
		return EDL_ColorkeySurface(Bitmap,c2,Rect);
	}
	if(m3>1){
		return EDL_ColorkeySurface(Bitmap,c3,Rect);
	}
	if(m4>1){
		return EDL_ColorkeySurface(Bitmap,c4,Rect);
	}
	return EDL_ColorkeySurface(Bitmap,c1,Rect);
}

/*! \brief Alphablends a bitmap onto a new surface using a colorkey
 *  \param Bitmap Source surface
 *  \param Key Color to filter
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,
		Uint32 Key,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Bitmap){
		// Get target rect
		SDL_Rect rect={0,0,Bitmap->w,Bitmap->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Bitmap)){
			if(SDL_LockSurface(Bitmap)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel by colorkey
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 kr=(Key>>24)&0xFF,kg=(Key>>16)&0xFF,kb=(Key>>8)&0xFF;
				Uint8 br=0,bg=0,bb=0,ba=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx>=0 && dy>=0){
					if(dx<=Bitmap->w && dy<=Bitmap->h){
						EDL_GetPixel_auto(Bitmap,
								x+rect.x,y+rect.y,
								&br,&bg,&bb,&ba);
						if(br==kr && bg==kg && bb==kb){
							ba=0;
						}
						else{
							ba=0xFF;
						}
					}
					EDL_SetPixel_auto(retval,x,y,br,bg,bb,ba);
				}
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Bitmap)){
			SDL_UnlockSurface(Bitmap);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;
}

/*! \brief Alphablends a bitmap onto a new surface using a b/w mask
 *  \param Bitmap Source surface
 *  \param Mask Mask surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_MaskSurface(SDL_Surface *Bitmap,
		SDL_Surface *Mask,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Bitmap){
		// Get target rect
		SDL_Rect rect={0,0,Bitmap->w,Bitmap->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Bitmap)){
			if(SDL_LockSurface(Bitmap)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(Mask)){
			if(SDL_LockSurface(Mask)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 br=0,bg=0,bb=0,ba=0xFF;
				Uint8 mr=0,mg=0,mb=0,ma=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx>=0 && dy>=0){
					if(dx<=Bitmap->w && dy<=Bitmap->h){
						EDL_GetPixel_auto(Bitmap,
								x+rect.x,y+rect.y,
								&br,&bg,&bb,&ba);
					}
					if(dx<=Mask->w && dy<=Mask->h){
						EDL_GetPixel_auto(Mask,
								x+rect.x,y+rect.y,
								&mr,&mg,&mb,&ma);
					}
					EDL_SetPixel_auto(retval,x,y,br,bg,bb,(mr+mg+mb)/3);
				}
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Bitmap)){
			SDL_UnlockSurface(Bitmap);
		}
		if(SDL_MUSTLOCK(Mask)){
			SDL_UnlockSurface(Mask);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;

}

/*! \brief Maps the alpha channel of a surface onto a new one
 *  \param Source Source surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_MapSurface(SDL_Surface *Source,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Source && Source->format->Amask){
		// Get target rect
		SDL_Rect rect={0,0,Source->w,Source->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Source)){
			if(SDL_LockSurface(Source)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 r=0,g=0,b=0,a=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx<=Source->w && dx>=0 && dy<=Source->h && dy>=0){
					EDL_GetPixel_auto(Source,x+rect.x,y+rect.y,&r,&g,&b,&a);
				}
				EDL_SetPixel_auto(retval,x,y,a,a,a,0xFF);
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Source)){
			SDL_UnlockSurface(Source);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;
}

/*! \brief Creates a new surface and copies the content from the source
 *  \param Source Source surface
 *  \param Rect X and Y is used as source while W and H destinates size
 *  \return New SDL_Surface containing source graphics
 */
SDL_Surface *EDL_CopySurface(SDL_Surface *Source,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Source){
		// Create destination
		retval=EDL_CreateSurface(Source->w,Source->h);
		if(Rect){
			SDL_Rect srect={Rect->x,Rect->y,Source->w,Source->h};
			SDL_Rect drect={0,0,Source->w,Source->h};
			EDL_BlitSurface(Source,&srect,retval,&drect);
		}
		else{
			EDL_BlitSurface(Source,0,retval,0);
		}
	}
	return retval;
}

#ifdef VILE_FEATURE_SCALER
/*! \brief Resizes one surface to another using HQ2X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ2X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq2x once
	static bool hq2x_init=false;
	if(!hq2x_init){
		InitHQ2X();
		hq2x_init=true;
	}

LogTest("a");
	SDL_Surface *tmpsrc=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w,src->h,16,0,0,0,0);
	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			(src->w*2)+4,(src->h*2)+4,16,0,0,0,0);
	EDL_BlitSurface(src,0,tmpsrc,0);
	if(SDL_MUSTLOCK(tmpsrc)){
		SDL_LockSurface(tmpsrc);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
LogTest("b:%d",tmpsrc->pitch);
	hq2x_32((unsigned char*)tmpsrc->pixels,
			(unsigned char*)tmpdst->pixels,
			tmpsrc->w,tmpsrc->h,tmpsrc->pitch);
LogTest("c");
	if(SDL_MUSTLOCK(tmpsrc)){
		SDL_UnlockSurface(tmpsrc);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpsrc);
	SDL_FreeSurface(tmpdst);
LogTest("DONE");
}

/*! \brief Resizes one surface to another using HQ2X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ3X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq3x once
	static bool hq3x_init=false;
	if(!hq3x_init){
		InitHQ3X();
		hq3x_init=true;
	}

	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w*3,src->h*3,16,0,0,0,0);
	if(SDL_MUSTLOCK(src)){
		SDL_LockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
	hq2x_32((unsigned char*)src->pixels,
			(unsigned char*)tmpdst->pixels,
			src->w,src->h,src->w*4);
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpdst);

}

/*! \brief Resizes one surface to another using HQ4X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ4X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq4x once
	static bool hq4x_init=false;
	if(!hq4x_init){
		InitHQ4X();
		hq4x_init=true;
	}

	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w*4,src->h*4,16,0,0,0,0);
	if(SDL_MUSTLOCK(src)){
		SDL_LockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
	hq4x_32((unsigned char*)src->pixels,
			(unsigned char*)tmpdst->pixels,
			src->w,src->h,src->w*4);
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpdst);
}
#endif

/*! \brief Resizes one surface to another
 *  \param Src Source surface
 *  \param Dst Destination surface
 *  \param Smooth Wether to smooth the surface
 */
void EDL_ResizeSurface(SDL_Surface *Src,SDL_Surface *Dst,bool Smooth){
	if(Src->w==Dst->w && Src->h==Dst->h){
		// Blit directly
		EDL_BlitSurface(Src,0,Dst,0);
	}
	else{
		// Resized blit
		SDL_Surface *zs=zoomSurface(Src,
				Dst->w/(double)Src->w,
				Dst->h/(double)Src->h,Smooth?1:0);
		if(zs){
			EDL_BlitSurface(zs,0,Dst,0);
			SDL_FreeSurface(zs);
		}
		else{
			LogError("Resized blit failed!");
			EDL_BlitSurface(Src,0,Dst,0);
		}
	}
}

/*! \brief Blends one surface onto another
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  Please note that the alpha values will be untouched in the result surface
 */
void EDL_BlendSurface(
		SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect){
	// Assert source and destination rects
	SDL_Rect s={0,0,src->w,src->h};
	SDL_Rect d={0,0,dst->w,dst->h};
	if(srcrect){
		s=*srcrect;
	}
	if(dstrect){
		d=*dstrect;
	}

	// Limit blit to available data
	d.w=EDL_MIN(d.w,s.w);
	d.h=EDL_MIN(d.h,s.h);
	s.w=EDL_MIN(d.w,s.w);
	s.h=EDL_MIN(d.h,s.h);

	SDL_BlitSurface(src,&s,dst,&d);
}

/*! \brief Copies one surface to another (Including alpha channel)
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  This method will copy all the specified pixels from source to destination
 *  surfaces, including the alpha channel when applicable, so no form of
 *  blending will take place.
 */
void EDL_BlitSurface(
		SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect){
	// Assert source and destination rects
	SDL_Rect s={0,0,src->w,src->h};
	SDL_Rect d={0,0,dst->w,dst->h};
	if(srcrect){
		s=*srcrect;
	}
	if(dstrect){
		d=*dstrect;
	}

	// Limit blit to available data
	d.w=EDL_MIN(d.w,s.w);
	d.h=EDL_MIN(d.h,s.h);
	s.w=EDL_MIN(d.w,s.w);
	s.h=EDL_MIN(d.h,s.h);

	// Pass call to SDL
	Uint32 flag=src->flags&SDL_SRCALPHA;
	if(flag){
		src->flags&=~(SDL_SRCALPHA);
		SDL_BlitSurface(src,&s,dst,&d);
		src->flags|=SDL_SRCALPHA;
	}
	else{
		SDL_BlitSurface(src,&s,dst,&d);
	}
}

/*! \brief Sets alphachannel to a fixed value
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param Alpha Alpha value
 */
void EDL_SetAlpha(SDL_Surface *src, SDL_Rect *srcrect,Uint8 Alpha){
	// Get best rect
	SDL_Rect srect={0,0,src->w,src->h};
	if(srcrect){
		srect.x=srcrect->x;
		srect.y=srcrect->y;
		srect.w=srcrect->w;
		srect.h=srcrect->h;
	}
	srect.x=EDL_LIMIT(srect.x,0,src->w);
	srect.y=EDL_LIMIT(srect.y,0,src->h);
	srect.w=EDL_LIMIT(srect.w,0,src->w);
	srect.h=EDL_LIMIT(srect.h,0,src->h);

	// Lock surfaces
	if(SDL_MUSTLOCK(src)){
		if(SDL_LockSurface(src)<0){
			return;
		}
	}

	// Force alpha (Assuming 32bit surface)
	srect.w+=srect.x;
	srect.h+=srect.y;
	for(int x=srect.x;x<srect.w;x++){
		for(int y=srect.y;y<srect.h;y++){
			Uint32 *pixel=(Uint32*)src->pixels+y*src->pitch/4+x;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			*pixel=((*pixel)&0xFFFFFF00)|Alpha;
#else
			*pixel=((*pixel)&0x00FFFFFF)|(Alpha<<24);
#endif
		}
	}

	// Unlock
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
}

