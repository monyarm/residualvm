/*! \class AudioFluid
 *	\brief AudioFluid implements Fluidsynth on top of a AudioBuffer object.
 */

#ifndef _AFLUID_H_
#define _AFLUID_H_

#include "audio.h"
#include "dstack.h"
#include <fluidsynth.h>

struct FLUID_FRAME {
	FLUID_FRAME(unsigned char *Buffer);
	~FLUID_FRAME();
	unsigned char *buffer;
};

class AudioFluid : public AudioBuffer{
	private:
		static void log(int,char *,void*);	//!< Callback for fluidsynth log
		DStack framebuffer;					//!< Stacks formatted frames
		SDL_mutex *datalock;				//!< Mutex for controldata
		SDL_mutex *framelock;				//!< Mutex for framebuffer
		fluid_settings_t *settings;			//!< Fluidsynth settings
		fluid_synth_t *synth;				//!< Fluidsynth synthesizer
		fluid_player_t *player;				//!< Fluidsynth player
		unsigned char *midibuffer;			//!< Buffer to MIDI data
		unsigned int midisize;				//!< Size of MIDI data
	public:
		AudioFluid();
		~AudioFluid();
		virtual bool Open(RWops *Resource);
		virtual void Mix(unsigned char *Buffer,int Length,double Volume);
		virtual bool Decode();
		virtual void Close();
};

#endif

