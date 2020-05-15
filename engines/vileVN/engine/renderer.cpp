#include "renderer.h"

Renderer::Renderer(Group *Widgets,Group *Animations,int Width,int Height){
	scaler=GS_DEFAULT;
	width=Width;
	height=Height;
	widgets=Widgets;
	animations=Animations;
	gamesurface=SDL_CreateRGBSurface(SDL_SWSURFACE,Width,Height,32,0,0,0,0);
}

Renderer::~Renderer(){
	SDL_FreeSurface(gamesurface);
}

/*! \brief Select scaling method
 *  \param Scaler Scaling method
 */
void Renderer::SetScaler(GRAPHIC_SCALER Scaler){
	scaler=Scaler;
}

/*! \brief Scales one surface onto another
 *  \param In Surface to scale
 *  \param Out Output surface
 */
void Renderer::Scale(SDL_Surface *In,SDL_Surface *Out){
	if(scaler==GS_DEFAULT && In->w==Out->w && In->h==Out->h){
		// Optimize for unresized blits
		EDL_BlitSurface(In,0,Out,0);
	}
	else if(scaler==GS_DEFAULT){
		// Default to software blits
		EDL_ResizeSurface(In,Out,true);
	}
#ifdef VILE_FEATURE_SCALER
	else if(scaler==GS_FAST){
		EDL_ResizeSurface(In,Out,false);
	}
	else if(scaler==GS_NEAREST){
		EDL_ResizeSurface(In,Out,true);
	}
	else if(scaler==GS_HQ2X){
		EDL_ScaleHQ2X(In,Out);
	}
	else if(scaler==GS_HQ3X){
		EDL_ScaleHQ3X(In,Out);
	}
	else if(scaler==GS_HQ4X){
		EDL_ScaleHQ4X(In,Out);
	}
#endif
	else{
		EDL_ResizeSurface(In,Out,true);
		LogError("Unknown scaler: %d",scaler);
	}
}

/*! \brief Renders graphics from scratch
 *  \param Surface Surface to render to
 *  \param Top Top layer to render
 *
 *  This method is a versatile interface that allows drawing any part of the
 *  widgets at any time. It is typically uncached and designed to be used for
 *  dumping screenshots for savegames etc.
 */
void Renderer::Render(SDL_Surface *Surface,int Top){
	// Unset global refresh and check if we need scaling
	SDL_Rect clip={0,0,width,height};
	Widget::GetGlobalRefresh(&clip);
	EDL_LIMIT(clip.w,0,width);
	EDL_LIMIT(clip.h,0,height);

	// Paint widgets to the surface
	int l=widgets->GetGroupCount()-1;
	Group *gptr=widgets->GetGroup(l);
	while(gptr){
		// Paint widgets
		Widget *wptr=gptr->GetWidget(gptr->GetWidgetCount()-1);
		while(wptr){
			if(wptr->GetVisible()){
				wptr->Copy(gamesurface);
			}
			wptr=wptr->PrevPtr;
		}

		// Check for top layer
		if((--l)<(int)Top){
			break;
		}
		gptr=gptr->PrevPtr;
	}

	// Scale output
	if(Surface->w!=width || Surface->h!=height){
		Scale(gamesurface,Surface);
	}
	else{
		EDL_BlitSurface(gamesurface,&clip,Surface,&clip);
	}
}

/*! \brief Updates graphics
 *  \param Surface Surface to render to
 *  \param Top Top layer to render
 *
 *  This method is the primary rendering interface for the ViLE core. It is
 *  meant to perform any caching transparantly and only paint unobstructed,
 *  visible widgets to save time and processing power. The rendered output
 *  can be either an animation or the currently visible widgets.
 *
 *  Some implementations might expect the Surface to be preloaded with 
 *  graphics from the previous run (eg. dirty rects) so use Render() if
 *  you just want a simple screendump.
 */
void Renderer::Update(SDL_Surface *Surface){
	if(animations->GetWidgetCount()){
		UpdateAnimations(Surface);
	}
	else{
		UpdateWidgets(Surface);
	}
}

/*! \brief Updates animation rendering
 *  \param Surface Surface to render to
 */
void Renderer::UpdateAnimations(SDL_Surface *Surface){
	// Animate current graphics directly to screen
	int n=animations->GetWidgetCount()-1;
	Animation *aptr=(Animation*)animations->GetWidget(n);
	if(aptr){
		// Get animation frame
		SDL_Surface *tmpsurface=EDL_CreateSurface(width,height);
		EDL_BlitSurface(gamesurface,0,tmpsurface,0);
		aptr->Copy(tmpsurface);
		Scale(tmpsurface,Surface);
			
		// Update base graphics and drop widget
		if(!aptr->Continue()){
			EDL_BlitSurface(tmpsurface,0,gamesurface,0);
			animations->DestroyWidget(aptr);
		}
		SDL_FreeSurface(tmpsurface);
	}
}

/*! \brief Updates widget graphics
 *  \param Surface Surface to render to
 *
 *  The default widget rendering is the traditional method of rendering all
 *  widgets (Idendical to the Render() method) which is very safe, but also
 *  very inefficient.
 */
void Renderer::UpdateWidgets(SDL_Surface *Surface){
	// Reset global refresh while checking it
	if(Widget::PeekGlobalRefresh()){
		Render(Surface,0);
	}
}

