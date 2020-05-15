//#include "vileVN/vile.h"

#include "base/plugins.h"

#include "engines/advancedDetector.h"

#define PROBESUF(name, platform) PROBESUFFULL(name, name,platform, Common::EN_ANY), \
	                             PROBESUFFULL(name, name,platform, Common::JA_JPN),   \
	                             PROBESUFFULL(name, name,platform, Common::RU_RUS)

#define PROBESUFFULL(name, name2, platform, lang)                                                         \
	{name, 0, AD_ENTRY1s("game.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},         \
	    {name, 0, AD_ENTRY1s("us.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},       \
	    {name, 0, AD_ENTRY1s("ml.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},       \
	    {name, 0, AD_ENTRY1s(name2 ".suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},   \
	    {name, 0, AD_ENTRY1s(name2 "us.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)}, \
	{ name, 0, AD_ENTRY1s(name2 "ml.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }

namespace VileVN {
//const char *VileVNEngine::getGameId() const { return _gameDescription->gameId; }
//Common::Platform VileVNEngine::getPlatform() const { return _gameDescription->platform; }

} // namespace VileVN

static const PlainGameDescriptor VileVNGames[] = {
    {"mayclub", "May Club", "WIP", "https://vndb.org/v190"},
    {"mayclub2", "May Club 2", "WIP", "https://vndb.org/v190"},
    {"nocIll", "Nocturnal Illusion", "WIP", "https://vndb.org/v190"},
    {"cres", "Crescendo ~Eien da to Omotte Ita Ano Koro~", "WIP", "https://vndb.org/v29"},
    {"sagara", "Sagara-sanchi no Etsuraku Life", "WIP", "https://vndb.org/v46"},
    {"yuki", "Yuki Sakura", "WIP", "https://vndb.org/v71"},
    {"kanaoka", "Kana ~Imouto~", "WIP", "https://vndb.org/v2"},
    {"hitomi", "Gimai - Hitomi", "WIP", "https://vndb.org/v31"},
    {"koneko", "Koneko Doumei", "WIP", "https://vndb.org/v153"},
    {"mesia", "Meshimase Idol", "WIP", "https://vndb.org/v141"},
    {0, 0, 0, 0}};

namespace VileVN {

static const ADGameDescription gameDescriptions[] = {
    //May Club 1
    {"mayclub",
     0,
     {{"may0.dat", 0, NULL, -1},
      {"may0.lst", 0, NULL, -1},
      {"cg/graphics.pak", 0, NULL, -1},
      {"wave/n02_88.wav", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformWindows,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},
    {"mayclub",
     0,
     {{"may0.dat", 0, NULL, -1},
      {"may0.lst", 0, NULL, -1},
      {"cg/graphics.pak", 0, NULL, -1},
      {"wave/n02_88.wav", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformPC98,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},
    {"mayclub",
     0,
     {{"may0.dat", 0, NULL, -1},
      {"may0.lst", 0, NULL, -1},
      {"cg/graphics.pak", 0, NULL, -1},
      {"wave/n02_88.wav", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformMacintosh,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},
    {"mayclub",
     0,
     {{"may0.dat", 0, NULL, -1},
      {"may0.lst", 0, NULL, -1},
      {"cg/graphics.pak", 0, NULL, -1},
      {"wave/n02_88.wav", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformLinux,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},

    //May Club 2
    {"mayclub2",
     0,
     {{"may0.dat", 0, NULL, -1},
      {"may0.lst", 0, NULL, -1},
      {"cg/graphics.pak", 0, NULL, -1},
      {"wave/n02_88.wav", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformWindows,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},

    //Nocturnal Illusion
    {"nocIll",
     0,
     {{"mug0.dat", 0, NULL, -1},
      {"mug0.lst", 0, NULL, -1},
      AD_LISTEND},
     Common::EN_ANY,
     Common::kPlatformWindows,
     ADGF_NO_FLAGS,
     GUIO1(GUIO_NONE)},

    //Crescendo
    PROBESUF("cres", Common::kPlatformWindows),
    PROBESUF("cres", Common::kPlatformMacintosh),
    PROBESUF("cres", Common::kPlatformLinux),

    //Sagara
    PROBESUF("sagara", Common::kPlatformWindows),

    //Yuki Sakura
    PROBESUF("yuki", Common::kPlatformWindows),

    //Kanao Okaeri
    PROBESUF("kanaoka", Common::kPlatformWindows),
    PROBESUF("kanaoka", Common::kPlatformMacintosh),
    PROBESUF("kanaoka", Common::kPlatformLinux),
    PROBESUF("kanaoka", Common::kPlatformPSP),

    PROBESUFFULL("kanaoka", "kana", Common::kPlatformWindows,Common::EN_ANY),
    PROBESUFFULL("kanaoka", "kana", Common::kPlatformMacintosh,Common::EN_ANY),
    PROBESUFFULL("kanaoka", "kana", Common::kPlatformLinux,Common::EN_ANY),
    PROBESUFFULL("kanaoka", "kana", Common::kPlatformPSP,Common::EN_ANY),

    //Hitomi
    PROBESUF("hitomi", Common::kPlatformWindows),

    //Cat Girl Alliance
    PROBESUF("koneko", Common::kPlatformWindows),

    //Idols Galore
    PROBESUF("mesia", Common::kPlatformWindows),

    AD_TABLE_END_MARKER};

} // End of namespace VileVN

class VileVNMetaEngine : public AdvancedMetaEngine {
public:
	VileVNMetaEngine() : AdvancedMetaEngine(VileVN::gameDescriptions, sizeof(ADGameDescription), VileVNGames) {
	}

	const char *getEngineId() const override {
		return "VileVN";
	}

	const char *getName() const override {
		return "VileVN";
	}

	const char *getOriginalCopyright() const override {
		return "";
	}

	bool hasFeature(MetaEngineFeature f) const override;
	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
};

bool VileVNMetaEngine::hasFeature(MetaEngineFeature f) const {
	return false;
}

bool VileVNMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc)
		///*engine = new VileVN::VileVNEngine(syst, desc);

		return desc != nullptr;
}

#if PLUGIN_ENABLED_DYNAMIC(VILEVN)
REGISTER_PLUGIN_DYNAMIC(VILEVN, PLUGIN_TYPE_ENGINE, VileVNMetaEngine);
#else
REGISTER_PLUGIN_STATIC(VILEVN, PLUGIN_TYPE_ENGINE, VileVNMetaEngine);
#endif
