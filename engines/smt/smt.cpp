#include "smt/formats/archive/cpk.h"
#include "smt/formats/video/pmsf.h"
#include "smt/formats/image/tmx.h"
#include "smt/formats/image/dds.h"

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

		const Graphics::PixelFormat *format = new Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);

		if (strcmp(getGameId(), "P3P") == 0)
		{
			//PSP
			//initGraphics(480, 272, format);
			initGraphics(1920, 1080, format);
		}
		else if (strcmp(getGameId(), "P4G") == 0)
		{
			//PSVita
			initGraphics(960, 554, format);
		}
		else
		{

			initGraphics(1920, 1080, format);
		}

		//_cpk.ReadFile("umd0.cpk");

		//PMSFFile _pmsf = PMSFFile();

		//_pmsf.ReadFile("p3opmv_p3p.pmsf");

		TMXFile _tmx("test/PSMT8.tmx");
		DDSFile _dds("test/DXT5.dds");

		//CVMArchive _bgm("BGM.CVM");
		//CVMArchive _data("DATA.CVM");
		//CVMArchive _btl("BTL.CVM");

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
		//OSystem::kTransactionSizeChangeFailed here
		//_system->endGFXTransaction();

		// Create debugger console. It requires GFX to be initialized
		_console = new Console(this);

		// Additional setup.
		debug("SMTEngine::init");

		Common::Event e;

		g_system->getEventManager()->pollEvent(e);
		g_system->delayMillis(10);

		Graphics::Surface *screen = g_system->lockScreen();
		screen->fillRect(Common::Rect(0, 0, g_system->getWidth(), g_system->getHeight()), 223330);

		Graphics::TransparentSurface *surfacetmx = _tmx.getSurface();
		surfacetmx->blit(*screen, 0, 0, 0);

		Graphics::TransparentSurface *surfacedds = _dds.getSurface();

		surfacedds->blit(*screen, 600, 0, 0);

		g_system->unlockScreen();
		g_system->updateScreen();

		while (!shouldQuit())
		{
			g_system->getEventManager()->pollEvent(e);
			g_system->delayMillis(10);
		}

		return Common::kNoError;
	}

} // End of namespace SMT
