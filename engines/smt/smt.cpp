#include "smt/formats/archive/cpk.h"
#include "smt/formats/video/pmsf.h"
#include "smt/formats/image/tmx.h"

#include "smt/smt.h"

namespace SMT
{

SMTEngine::SMTEngine(OSystem *syst, const ADGameDescription *desc)
	: Engine(syst), _gameDescription(desc), _console(nullptr)
{
	// Put your engine in a sane state, but do nothing big yet;
	// in particular, do not load data from files; rather, if you
	// need to do such things, do them from run().

	// Do not initialize graphics here
	// Do not initialize audio devices here

	// However this is the place to specify all default directories
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "sound/pmsf");

	// Here is the right place to set up the engine specific debug channels
	DebugMan.addDebugChannel(kSMTDebug, "example", "this is just an example for a engine specific debug channel");
	DebugMan.addDebugChannel(kSMTDebug2, "example2", "also an example");

	// Don't forget to register your random source
	_rnd = new Common::RandomSource("smt");

	debug("SMTEngine::SMTEngine");
}

SMTEngine::~SMTEngine()
{
	debug("SMTEngine::~SMTEngine");

	// Dispose your resources here
	delete _rnd;

	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();
}

Common::Error SMTEngine::run()
{
	// Initialize graphics using following:

	Common::List<Graphics::PixelFormat> formats = g_system->getSupportedFormats();
	Graphics::PixelFormat *format = new Graphics::PixelFormat();
	*format = Graphics::createPixelFormat<8888>();

	if (getGameId() == "P3P")
	{
		//PSP
		//initGraphics(480, 272, format);
		initGraphics(1920, 1080, format);
	} else if (getGameId() == "P4G")
	{
		//PSP
		initGraphics(960, 554, format);
	}
	else
	{
		
		initGraphics(1920, 1080, format);
	}
	

	//Graphics::PixelFormat format = g_system->getScreenFormat();
	debug(format->toString().c_str());

	//CPKFile _cpk = CPKFile();

	//_cpk.ReadFile("umd0.cpk");

	//PMSFFile _pmsf = PMSFFile();

	//_pmsf.ReadFile("p3opmv_p3p.pmsf");

	TMXFile _tmx = TMXFile();

	_tmx.ReadFile("test.tmx");

	Common::Event e;
	while (!shouldQuit())
	{
		g_system->getEventManager()->pollEvent(e);
		g_system->delayMillis(10);

		Graphics::Surface *screen = g_system->lockScreen();
		screen->fillRect(Common::Rect(0, 0, g_system->getWidth(), g_system->getHeight()), 0);

		Graphics::ManagedSurface *surface = _tmx.getSurface(); // = tmxData

		int w = CLIP<int>(surface->w, 0, g_system->getWidth());
		int h = CLIP<int>(surface->h, 0, g_system->getHeight());

		int x = (g_system->getWidth() - w) / 2;
		int y = (g_system->getHeight() - h) / 2;

		screen->copyRectToSurface(*surface, x, y, Common::Rect(0, 0, w, h));

		g_system->unlockScreen();
		g_system->updateScreen();
	}

	// You could use backend transactions directly as an alternative,
	// but it isn't recommended, until you want to handle the error values
	// from OSystem::endGFXTransaction yourself.
	// This is just an example template:
	//_system->beginGFXTransaction();
	//	// This setup the graphics mode according to users seetings
	//	initCommonGFX(false);
	//
	//	// Specify dimensions of game graphics window.
	//	// In this example: 320x200
	//	_system->initSize(320, 200);
	//FIXME: You really want to handle
	//OSystem::kTransactionSizeChangeFailed here
	//_system->endGFXTransaction();

	// Create debugger console. It requires GFX to be initialized
	_console = new Console(this);

	// Additional setup.
	debug("SMTEngine::init");

	// Your main even loop should be (invoked from) here.
	debug("SMTEngine::go: Hello, World!");

	// This test will show up if -d1 and --debugflags=example are specified on the commandline
	debugC(1, kSMTDebug, "Example debug call");

	// This test will show up if --debugflags=example or --debugflags=example2 or both of them and -d3 are specified on the commandline
	debugC(3, kSMTDebug | kSMTDebug2, "Example debug call two");

	return Common::kNoError;
}

} // End of namespace SMT
