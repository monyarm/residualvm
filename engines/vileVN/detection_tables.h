
#include "base/plugins.h"

#include "engines/advancedDetector.h"

#define PROBESUF(name, platform) PROBESUFFULL(name, name, platform, Common::EN_ANY), \
	                             PROBESUFFULL(name, name, platform, Common::JA_JPN), \
	                             PROBESUFFULL(name, name, platform, Common::RU_RUS)

#define PROBESUFFULL(name, name2, platform, lang)                                                         \
	{name, 0, AD_ENTRY1s("game.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},         \
	    {name, 0, AD_ENTRY1s("us.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},       \
	    {name, 0, AD_ENTRY1s("ml.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},       \
	    {name, 0, AD_ENTRY1s(name2 ".suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},   \
	    {name, 0, AD_ENTRY1s(name2 "us.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)}, \
	{ name, 0, AD_ENTRY1s(name2 "ml.suf", NULL, -1), lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }

#define PROBEJASTUSANAME(name) PROBEJASTUSAFULL(name, Common::kPlatformWindows, Common::EN_ANY)

#define PROBEJASTUSAFULL(name, platform, lang)                                                        \
	{name, 0, {{"frames.pck", 0, NULL, -1},{"data.pck", 0, NULL, -1}, {"images.pck", 0, NULL, -1}, AD_LISTEND}, lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)}

#define PROBEJASTUSA()              \
	PROBEJASTUSANAME("meisou"),     \
	    PROBEJASTUSANAME("sakura"), \
	    PROBEJASTUSANAME("sanshi")

#define PROBEDIVIDEADPLATLANG(platform, lang)                                        \
	{ "dividead", 0, {{"sg.dl1", 0, NULL, -1}, {"wv.dl1", 0, NULL, -1}, AD_LISTEND}, \
	  lang,                                                                          \
	  platform,                                                                      \
	  ADGF_NO_FLAGS,                                                                 \
	  GUIO1(GUIO_NONE) }

#define PROBEDIVIDEADPLAT(platform)                      \
	PROBEDIVIDEADPLATLANG(platform, Common::EN_ANY),     \
	    PROBEDIVIDEADPLATLANG(platform, Common::PT_BRA), \
	    PROBEDIVIDEADPLATLANG(platform, Common::PT_POR), \
	    PROBEDIVIDEADPLATLANG(platform, Common::JA_JPN), \
	    PROBEDIVIDEADPLATLANG(platform, Common::ES_ESP)

#define PROBEDIVIDEAD()                           \
	PROBEDIVIDEADPLAT(Common::kPlatformWindows),  \
	    PROBEDIVIDEADPLAT(Common::kPlatformPSP),  \
	    PROBEDIVIDEADPLAT(Common::kPlatformXbox), \
	    PROBEDIVIDEADPLAT(Common::kPlatformDreamcast)

#define PROBEKANAOKA()                                                               \
	PROBESUF("kanaoka", Common::kPlatformWindows),                                   \
	    PROBESUF("kanaoka", Common::kPlatformMacintosh),                             \
	    PROBESUF("kanaoka", Common::kPlatformLinux),                                 \
	    PROBESUF("kanaoka", Common::kPlatformPSP),                                   \
	    PROBESUFFULL("kanaoka", "kana", Common::kPlatformWindows, Common::EN_ANY),   \
	    PROBESUFFULL("kanaoka", "kana", Common::kPlatformMacintosh, Common::EN_ANY), \
	    PROBESUFFULL("kanaoka", "kana", Common::kPlatformLinux, Common::EN_ANY),     \
	    PROBESUFFULL("kanaoka", "kana", Common::kPlatformPSP, Common::EN_ANY)

#define PROBEMAYCLUBFULL(platform, lang) \
	{ "mayclub", 0, {{"may0.dat", 0, NULL, -1}, {"may0.lst", 0, NULL, -1}, {"cg/graphics.pak", 0, NULL, -1}, {"wave/n02_88.wav", 0, NULL, -1}, AD_LISTEND}, lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }
#define PROBEMAYCLUB2FULL(platform, lang) \
	{ "mayclub", 0, {{"may0.dat", 0, NULL, -1}, {"may0.lst", 0, NULL, -1}, {"cg/graphics.pak", 0, NULL, -1}, {"wave/n02_88.wav", 0, NULL, -1}, AD_LISTEND}, lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }
#define PROBEMAYCLUBLANG(lang)                             \
	PROBEMAYCLUBFULL(Common::kPlatformWindows, lang),      \
	    PROBEMAYCLUB2FULL(Common::kPlatformWindows, lang), \
	    PROBEMAYCLUBFULL(Common::kPlatformLinux, lang),    \
	    PROBEMAYCLUBFULL(Common::kPlatformPC98, lang),     \
	    PROBEMAYCLUBFULL(Common::kPlatformMacintosh, lang)

#define PROBEMAYCLUB()                    \
	PROBEMAYCLUBLANG(Common::EN_ANY),     \
	    PROBEMAYCLUBLANG(Common::JA_JPN), \
	    PROBEJASTUSANAME("mayclub")

#define PROBENOCTURNALFULL(platform, lang) \
	{ "nocIll",                            \
	  0,                                   \
	  AD_ENTRY1s("mug0.dat", NULL, -1),    \
	  lang,                                \
	  platform,                            \
	  ADGF_NO_FLAGS,                       \
	  GUIO1(GUIO_NONE) }

#define PROBENOCTURNALLANG(lang)                          \
	PROBENOCTURNALFULL(Common::kPlatformWindows, lang),   \
	    PROBENOCTURNALFULL(Common::kPlatformLinux, lang), \
	    PROBENOCTURNALFULL(Common::kPlatformPC98, lang),  \
	    PROBENOCTURNALFULL(Common::kPlatformMacintosh, lang)

#define PROBENOCTURNAL() \   
PROBENOCTURNALLANG(Common::EN_ANY),                          \
	                     PROBENOCTURNALLANG(Common::JA_JPN), \
	                     PROBENOCTURNALLANG(Common::PT_BRA), \   
PROBENOCTURNALLANG(Common::PT_POR),                          \
	                     PROBENOCTURNALLANG(Common::RU_RUS), \
	                     PROBENOCTURNALLANG(Common::FR_FRA), \  
PROBEJASTUSANAME("noctIll")

#define PROBESAGARA()                                                           \
	PROBESUFFULL("sagara", "sagara", Common::kPlatformWindows, Common::EN_ANY), \
	    PROBESUFFULL("sagara", "sagara", Common::kPlatformWindows, Common::JA_JPN)

#define PROBECRESCENDOLANG(lang) \
	PROBESUFFULL("cres", "cres", Common::kPlatformWindows, lang)

#define PROBECRESCENDO()                    \
	PROBECRESCENDOLANG(Common::EN_ANY),     \
	    PROBECRESCENDOLANG(Common::JA_JPN), \
	    PROBECRESCENDOLANG(Common::RU_RUS), \
	    PROBECRESCENDOLANG(Common::ZH_ANY)

#define PROBEYUKI()                                                         \
	PROBESUFFULL("yuki", "yuki", Common::kPlatformWindows, Common::EN_ANY), \
	    PROBESUFFULL("yuki", "yuki", Common::kPlatformWindows, Common::JA_JPN)

#define PROBEHITOMI()                                                           \
	PROBESUFFULL("hitomi", "hitomi", Common::kPlatformWindows, Common::EN_ANY), \
	    PROBESUFFULL("hitomi", "hitomi", Common::kPlatformWindows, Common::JA_JPN)

#define PROBEKONEKO()                                                           \
	PROBESUFFULL("koneko", "koneko", Common::kPlatformWindows, Common::EN_ANY), \
	    PROBESUFFULL("koneko", "koneko", Common::kPlatformWindows, Common::JA_JPN)

#define PROBEMESIA()                                                          \
	PROBESUFFULL("mesia", "mesia", Common::kPlatformWindows, Common::EN_ANY), \
	    PROBESUFFULL("mesia", "mesia", Common::kPlatformWindows, Common::JA_JPN), \ 
    PROBESUFFULL("mesia", "mesia", Common::kPlatformWindows, Common::RU_RUS)

#define PROBETRUELOVEFULL(platform, lang) \
	{ "truelove",                         \
	  0,                                  \
	  {{"eff", 0, NULL, -1},              \
	   {"mrs", 0, NULL, -1},              \
	   {"date", 0, NULL, -1},             \
	   {"datg", 0, NULL, -1},             \
	   AD_LISTEND},                       \
	  lang,                               \
	  platform,                           \
	  ADGF_NO_FLAGS,                      \
	  GUIO1(GUIO_NONE) }

#define PROBETRUELOVELANG(lang)                        \
	PROBETRUELOVEFULL(Common::kPlatformWindows, lang), \
	    PROBETRUELOVEFULL(Common::kPlatformPC98, lang)

#define PROBETRULOVE()                     \
	PROBETRUELOVELANG(Common::EN_ANY),     \
	    PROBETRUELOVELANG(Common::JA_JPN), \
	    PROBETRUELOVELANG(Common::PT_BRA), \
	    PROBETRUELOVELANG(Common::PT_POR), \
	    PROBETRUELOVELANG(Common::DE_DEU), \
	    PROBETRUELOVELANG(Common::ES_ESP), \
	    PROBETRUELOVELANG(Common::RU_RUS)

#define PROBERIOCHIPARC(name, platform, lang) \
	{ name, 0, {{"rio.arc", 0, NULL, -1}, {"chio.arc", 0, NULL, -1}, AD_LISTEND}, lang, platform, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }

#define PROBEYUMEMIRU()                                                        \
	PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::EN_ANY),     \
	    PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::JA_JPN), \
	    PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::ZH_ANY), \
	    PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::ES_ESP), \
	    PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::RU_RUS), \
	    PROBERIOCHIPARC("yumemiru", Common::kPlatformWindows, Common::KO_KOR)

#define PROBECRITPOINT()                                                            \
	PROBERIOCHIPARC("criticalpoint", Common::kPlatformWindows, Common::EN_ANY),     \
	    PROBERIOCHIPARC("criticalpoint", Common::kPlatformWindows, Common::JA_JPN), \
	    PROBERIOCHIPARC("criticalpoint", Common::kPlatformWindows, Common::RU_RUS)

#define PROBETOKIMEKI()                                                                                                                                                                                \
	{"tokimeki", 0, {{"scene00.bdt", 0, NULL, -1}, {"353-103.zbm", 0, NULL, -1}, {"indexw.dat", 0, NULL, -1}, AD_LISTEND}, Common::EN_ANY, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)}, \
	{ "tokimeki", 0, {{"scene00.bdt", 0, NULL, -1}, {"353-103.zbm", 0, NULL, -1}, {"indexw.dat", 0, NULL, -1}, AD_LISTEND}, Common::JA_JPN, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }

#define PROBEXCH1()                                                                                                                                                                                     \
	{"xch1", 0, {{"scene00.bdt", 0, NULL, -1}, {"207-30l2.zbm", 0, NULL, -1}, {"indexw.dat", 0, NULL, -1}, AD_LISTEND}, Common::EN_ANY, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)},     \
	    {"xch1", 0, {{"scene00.bdt", 0, NULL, -1}, {"207-30l2.zbm", 0, NULL, -1}, {"indexw.dat", 0, NULL, -1}, AD_LISTEND}, Common::JA_JPN, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)}, \
	{                                                                                                                                                                                                   \
		"xch1", 0, {{"scene00.bdt", 0, NULL, -1}, {"207-30l2.zbm", 0, NULL, -1}, {"indexw.dat", 0, NULL, -1}, AD_LISTEND},                                                                              \
		    Common::RU_RUS, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE)                                                                                                                   \
	}

#define PROBEXCH3() \
	{ "xch3", 0, {{"xc3.sce", 0, NULL, -1}, AD_LISTEND}, Common::EN_ANY, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) },\
	{ "xch3", 0, {{"xc3.sce", 0, NULL, -1}, AD_LISTEND}, Common::JA_JPN, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) },\
	{ "xch3", 0, {{"xc3.sce", 0, NULL, -1}, AD_LISTEND}, Common::RU_RUS, Common::kPlatformWindows, ADGF_NO_FLAGS, GUIO1(GUIO_NONE) }

namespace VileVN {

static const ADGameDescription gameDescriptions[] = {
    PROBEMAYCLUB(),
    PROBENOCTURNAL(),
    PROBECRESCENDO(),
    PROBESAGARA(),
    PROBEYUKI(),
    PROBEKANAOKA(),
    PROBEHITOMI(),
    PROBEKONEKO(),
    PROBEMESIA(),
    PROBEJASTUSA(),
    PROBEDIVIDEAD(),
    PROBETRULOVE(),
    PROBEYUMEMIRU(),
    PROBECRITPOINT(),
    PROBETOKIMEKI(),
    PROBEXCH1(),
	PROBEXCH3(),

    AD_TABLE_END_MARKER};

} // End of namespace VileVN

static const PlainGameDescriptor VileVNGames[] = {
    {"mayclub", "May Club", "WIP", "https://vndb.org/v190"},
    {"mayclub2", "May Club 2", "WIP", "https://vndb.org/v190"},
    {"nocIll", "Mugen Yasoukyoku", "WIP", "https://vndb.org/v190"},
    {"cres", "Crescendo ~Eien da to Omotte Ita Ano Koro~", "WIP", "https://vndb.org/v29"},
    {"sagara", "Sagara-sanchi no Etsuraku Life", "WIP", "https://vndb.org/v46"},
    {"yuki", "Yuki Sakura", "WIP", "https://vndb.org/v71"},
    {"kanaoka", "Kana ~Imouto~", "WIP", "https://vndb.org/v2"},
    {"hitomi", "Gimai - Hitomi", "WIP", "https://vndb.org/v31"},
    {"koneko", "Koneko Doumei", "WIP", "https://vndb.org/v153"},
    {"mesia", "Meshimase Idol", "WIP", "https://vndb.org/v141"},
    {"meisou", "Meisou Toshi", "WIP", "https://vndb.org/v843"},
    {"sakura", "Sakura no Kisetsu", "WIP", "https://vndb.org/v206"},
    {"sanshi", "San Shimai", "WIP", "https://vndb.org/v113"},
    {"dividead", "Divi-Dead", "WIP", "https://vndb.org/v119"},
    {"truelove", "True Love ~Jun'ai Monogatari~", "WIP", "https://vndb.org/v241"},
    {"yumemiru", "Yume Miru Kusuri", "WIP", "https://vndb.org/v44"},
    {"criticalpoint", "Rinkaiten ~Critical Point~", "WIP", "https://vndb.org/v145"},
    {"tokimeki", "Tokimeki Check-In!", "WIP", "https://vndb.org/v111"},
    {"xch1", "X-Change 1", "WIP", "https://vndb.org/v43"},
    {"xch3", "X-Change 3", "WIP", "https://vndb.org/v129"},
    {0, 0, 0, 0}};
