/*! \class EngineMixer
 *  \brief Embeds audio playback and decoding to the engine object
 *
 *  This class encapsulates SDL_mixer functionality and facilitates music,
 *  voice and sfx clips to be played. You can use as many channels as you
 *  want to, but only one clip per channel at a time.
 *
 *  This class is basically a driver class for the decoder implementations
 *  in the media/ folder (See Audio class). We currently support FFMPEG and 
 *  Fluidsynth which should serve most purposes on desktop computers atleast.
 */

#ifndef _EMIXER_H_
#define _EMIXER_H_

#include "ebase.h"
#include "../media/audio.h"

#define AUDIO_CHANNELS	32

#ifdef VILE_FEATURE_FFMPEG
#include "../media/affmpeg.h"
#endif
#ifdef VILE_FEATURE_FLUIDSYNTH
#include "../media/afluid.h"
#endif

class EngineMixer : public EngineBase
{
	private:
		// Audio hardware interface
		static void audio_callback(void *udata,Uint8 *stream,int len);
		static int audio_thread(void *arg);
		SDL_Thread *thread_pointer;
		bool thread_lock;
			 
		// Declare Audio Channels
		struct AudioChannel {
			AudioChannel();
			~AudioChannel();
			AudioBuffer *buffer;
			SDL_mutex *mutex;
			bool enabled;
			double volume;
		}musicbuffer,soundbuffer[AUDIO_CHANNELS];

		// Internal handling
		SDL_AudioSpec hardware;
#ifndef VILE_BUILD_SDLHG
		SDL_CD *cdrom;
#endif
	public:
		EngineMixer(int Width,int Height);
		~EngineMixer();

		// Mixer API
		bool PlayCDDA(int Track);
		bool PlayMusic(RWops *Music);
		void PauseMusic();
		void ResumeMusic();
		void StopMusic();
		bool SetVolumeMusic(int Volume);
		int GetVolumeMusic();
		bool PlaySound(RWops *Sound,int Channel,int Replays=0);
		void StopSound(int Channel=-1);
		bool SetVolumeSound(int Channel,int Volume);
		int GetVolumeSound(int Channel);

		// Disable or enable audio (ie. muting)
		void SetMusicEnabled(bool Enabled);
		bool GetMusicEnabled();
		void SetSoundEnabled(int Channel,bool Enabled);
		bool GetSoundEnabled(int Channel);
};

#endif


