#ifndef SMT_H
#define SMT_H
 
#include "common/random.h"
#include "engines/engine.h"
#include "gui/debugger.h"
#include "common/scummsys.h"

#include "common/system.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/file.h"
#include "common/array.h"
#include "common/fs.h"
#include "common/str.h"
#include "common/events.h"
#include "common/rect.h"
#include "graphics/palette.h"
#include "graphics/surface.h"
#include "engines/util.h"

#include "engines/smt/gfx.h"

struct ADGameDescription;

 
namespace SMT {
 
class Sound;
class Console;
class Renderer;
class FrameLimiter;

// our engine debug channels
enum {
	kSMTDebug = 1 << 0,
	kSMTDebug2 = 1 << 1
	// next new channel must be 1 << 2 (4)
	// the current limitation is 32 debug channels (1 << 31 is the last one)
};
 
class SMTEngine : public Engine {
public:
	SMTEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~SMTEngine();
 
	virtual Common::Error run();

	// Detection related functions
	const ADGameDescription *_gameDescription;
	const char *getGameId() const;
	Common::Platform getPlatform() const;

	Renderer *_gfx;
	Sound *_sound;
 
private:
	Console *_console;
 	FrameLimiter *_frameLimiter;
	// We need random numbers
	Common::RandomSource *_rnd;
};
 
// Example console class
class Console : public GUI::Debugger {
public:
	Console(SMTEngine *vm) {}
	virtual ~Console(void) {}
};
 
} // End of namespace SMT
 
#endif