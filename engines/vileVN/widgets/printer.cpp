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

#include "printer.h"

Printer::Printer(int X,int Y,int Width,int Height) : Widget(X,Y,Width,Height){
	// Preset values
	stext=0;
	index=0;
	interval=50;
	charcount=0;
	wordwrap=true;
	clearscreen=false;

	// Create default font 
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::Printer(SDL_Rect Pos) : Widget(Pos){
	// Preset values
	stext=0;
	index=0;
	interval=50;
	charcount=0;
	wordwrap=true;
	clearscreen=false;

	// Create default font 
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::Printer() : Widget(){
	// Preset values
	stext=0;
	index=0;
	interval=50;
	charcount=0;
	wordwrap=true;
	clearscreen=false;

	// Create default font 
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::~Printer(){
	if(stext){
		SDL_FreeSurface(stext);
	}
	if(font_ttf){
		TTF_CloseFont(font_ttf);
	}
}

void Printer::SetWordwrap(bool Enable){
	wordwrap=Enable;
}

void Printer::SetFontFace(uString Name){
	// Verify font existence
	if(Name.length() && Name!=font_name){
		FILE *tf=fopen(Name.c_str(),"r");
		if(tf){
			fclose(tf);
			font_name=Name;
			if(font_ttf){
				TTF_CloseFont(font_ttf);
			}
			font_ttf=TTF_OpenFont(font_name.c_str(),font_size);
			if(!font_ttf){
				LogError("Failed to open font: %s",font_name.c_str());
			}
		}
		else{
			LogError("Failed to find font: %s",font_name.c_str());
		}
	}
}

void Printer::SetFontSize(int Size){
	// Register new size
	font_size=Size;

	// Attempt to recreate font
	if(font_name.length()){
		if(font_ttf){
			TTF_CloseFont(font_ttf);
		}
		font_ttf=TTF_OpenFont(font_name.c_str(),font_size);
	}
}

void Printer::SetBackgroundColor(Uint32 Color){
	back_color.r=(Color>>24)&0xFF;
	back_color.g=(Color>>16)&0xFF;
	back_color.b=(Color>>8)&0xFF;
	back_alpha=Color&0xFF;
}

void Printer::SetFontColor(Uint32 Color){
	font_color.r=(Color>>24)&0xFF;
	font_color.g=(Color>>16)&0xFF;
	font_color.b=(Color>>8)&0xFF;
}

void Printer::SetFontColor(Uint8 Red,Uint8 Green,Uint8 Blue){
	font_color.r=Red;
	font_color.g=Green;
	font_color.b=Blue;
}

void Printer::SetFontColor(SDL_Color Color){
	font_color.r=Color.r;
	font_color.g=Color.g;
	font_color.b=Color.b;
}

void Printer::SetFontStyle(int Style){
	font_style=Style;
	if(font_ttf){
		TTF_SetFontStyle(font_ttf,font_style);
	}
}

void Printer::SetFontShadow(int X,int Y,SDL_Color Color){
	LogMessage("SETFONTSHADOW");
}

void Printer::SetFontGlow(int Glow,SDL_Color Color){
	LogMessage("SETFONTGLOW");
}

/*! \brief Prints remaining text immidiatly
 *  \return True if text was rendered
 */
bool Printer::Skip(){
	bool retval=font_ttf && text.length();
	if(retval){
		while(text.length()){
			while(printnext());
		}
		Refresh();
	}
	return retval;
}

/*! \brief Prints remaining text immidiatly, but wait at full display
 *  \return True if text was rendered
 */
bool Printer::SkipScreen(){
	bool retval=text.length();
	if(retval){
		if(clearscreen){
			clearscreen=false;
			ClearScreen();
			start=SDL_GetTicks();
		}
		while(printnext());
		Refresh();
	}
	return retval;
}


/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Printer::Continue(){
	return text.length();
}

/*! \brief Clear text and display
 */
void Printer::Clear(){
	if(stext){
		SDL_FreeSurface(stext);
		stext=0;
		Refresh();
	}
	index=0;
	text="";
	curtext="";
	charcount=0;
}

/*! \brief Clear display only
 */
void Printer::ClearScreen(){
	if(stext){
		// Drop printed text
		SDL_FreeSurface(stext);
		stext=0;
		Refresh();
	}
	if(text.length()>index){
		// Clear currently printed text
		start=SDL_GetTicks();
		text=text.substr(index);
		index=0;
	}
	else{
		// Clear all text
		text="";
		index=0;
	}
	curtext="";
	charcount=0;
	trect.y=0;
	trect.x=0;
}

/*! \brief Total number of characters to be printed
 *  \returns Total number of characters to be printed
 */
int Printer::GetLength(){
	return charcount;
}

/*! \brief Number of characters not yet printed
 *  \returns Number of characters not yet printed
 */
int Printer::GetRemaining(){
	return EDL_MAX(text.length()-index,0);
}

int Printer::GetInterval(){
	return interval;
}

void Printer::SetInterval(Uint32 Interval){
	interval=Interval;
}

void Printer::Newline(){
	if(font_ttf){
		if(text.length()){
			text+="\n";
		}
		else{
			int w,h;
			if(!TTF_SizeText(font_ttf,"A",&w,&h)){
				trect.x=0;
				trect.y+=h;
			}
		}
		Refresh();
	}
}

void Printer::Reprint(int Interval){
	// Complete current text
	Skip();

	// Start over
	start=SDL_GetTicks();
	uString curtextb=curtext;
	Clear();
	text=curtextb;
	curtext="";
	index=0;

	// Append textlength and override interval
	charcount+=text.length();
	if(Interval>=0){
		interval=Interval;
	}
	Refresh();

}

/*! \brief Prints text (Appended to existing)
 *  \param Text Text to be printed
 *  \param Interval Delay between each character (-1=Use preset value)
 */
void Printer::Print(uString Text,int Interval){
	if(text.length()){
		// Append to existing text
		text+=Text;
	}
	else{
		// Prepare printing data
		start=SDL_GetTicks();
		text=Text;
		index=0;
	}

	// Append textlength and override interval
	charcount+=Text.length();
	if(Interval>=0){
		interval=Interval;
	}
	Refresh();
}

void Printer::Autosize(uString Text){
	if(font_ttf){
		int width,height;
		if(!TTF_SizeText(font_ttf,Text.c_str(),&width,&height)){
			Resize(width,height);
		}
	}
}

bool Printer::GetTextSize(uString Text,int *Width,int *Height){
	bool retval=false;
	if(font_ttf){
		retval=!TTF_SizeText(font_ttf,Text.c_str(),Width,Height);
	}
	return retval;
}

/*! \brief Prints next character to the internal surface
 *  \returns True if text was added
 */
bool Printer::printnext(){
	bool retval=false;
	if(font_ttf && text.length()){
		// Assert a 32bit RGBA surface for transparent text
		if(!stext){
			stext=EDL_CreateSurface(pos.w,pos.w);
			trect.x=0;
			trect.y=0;
		}

		// Check for special characters 
		while(index<text.length() && 
				(text[index]=='\r' ||
				text[index]=='\n' ||
				text[index]=='\t')){
			if(text[index]=='\r' && text[index+1]=='\n'){
				index++;
			}
			else if(text[index]=='\r' || text[index]=='\n'){
				index++;
				int w,h;
				if(!TTF_SizeText(font_ttf,"A",&w,&h)){
					trect.x=0;
					trect.y+=h;
				}
			}
			else if(text[index]=='\t'){
				index++;
				int w,h;
				if(!TTF_SizeText(font_ttf,"A",&w,&h)){
					trect.x+=(w*4);
				}
			}
		}

		// Check for horizontal wordwrap
		if(wordwrap && index<text.length() && text[index]==' '){
			unsigned int end=index+1;
			for(;end<text.length() && text[end]!=' ' && text[end]!='\n';end++);
			uString word=text.substr(index+1,end-index);
			int w,h;
			if(!TTF_SizeText(font_ttf,word.c_str(),&w,&h)){
				if(trect.x && trect.x+w>pos.w){
					// Wrap text and drop whitespace
					trect.x=0;
					trect.y+=h;
					index++;
				}
			}
		}

		// Check for vertical wordwrap
		if(wordwrap && index<text.length()){
			int w,h;
			if(!TTF_SizeText(font_ttf,"A",&w,&h)){
				if(trect.y && trect.y+(h*2)>pos.h){
					clearscreen=true;
					return false;
				}
			}
		}

		// Get next character
		if(index<text.length()){
			uString print=text.substr(index++,1);
			curtext+=print;
			int w,h;
			if(!TTF_SizeText(font_ttf,print.c_str(),&w,&h)){
				trect.w=w;
				trect.h=h;
			}

			// Check for ending
			if(index==text.length()){
				text="";
				index=0;
			}

			// Experimental font rendering
			SDL_Surface *sp=0;
			if((sp=EDL_RenderText(font_ttf,print,font_color))){
				// Copy text to image
				EDL_BlitSurface(sp,0,stext,&trect);
				SDL_FreeSurface(sp);
				trect.x+=trect.w;
				retval=true;
			}
			else {
				LogError("Failed to render font: %s",TTF_GetError());
			}
		}
	}
	return retval;
}

void Printer::Copy(SDL_Surface *Dst){
	if(Dst){
		// Update background
		if(back_alpha){
			boxRGBA(Dst,pos.x,pos.y,pos.x+pos.w,pos.y+pos.h,
					back_color.r,back_color.g,
					back_color.b,back_alpha);
		}

		// Update delayed text
		bool refresh=text.length();
		if(refresh && !clearscreen){
			if(interval){
				Uint32 now=SDL_GetTicks();
				if(now<start){
					start=SDL_GetTicks();
				}
				while(text.length() && 
						now>start+(interval*index) && 
						(refresh=printnext()));
			}
			else{
				while(text.length() && (refresh=printnext()));
			}

			// Force refresh until all text has been printed
			Refresh();
		}
		if(simage){
			// Use widget graphics
			Widget::Copy(Dst);
		}
		if(stext){
			// Blend in transparant text
			SDL_Rect tpos=pos;
			EDL_BlendSurface(stext,0,Dst,&tpos);
		}
	}
}


