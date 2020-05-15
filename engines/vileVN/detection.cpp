#include "vileVN/vileVN.h"

#include "detection_tables.h"

namespace VileVN {
//const char *VileVNEngine::getGameId() const { return _gameDescription->gameId; }
//Common::Platform VileVNEngine::getPlatform() const { return _gameDescription->platform; }

} // namespace VileVN

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

	if(strcmp(desc->filesDescriptions->fileName, "frames.pck")){
		//*engine - new VileVN::JASTUSAEngine(syst,desc);
	}

	if (desc)
		*engine = new VileVN::VileVNEngine(syst, desc);

	return desc != nullptr;
}

#if PLUGIN_ENABLED_DYNAMIC(VILEVN)
REGISTER_PLUGIN_DYNAMIC(VILEVN, PLUGIN_TYPE_ENGINE, VileVNMetaEngine);
#else
REGISTER_PLUGIN_STATIC(VILEVN, PLUGIN_TYPE_ENGINE, VileVNMetaEngine);
#endif
