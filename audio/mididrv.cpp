/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/config-manager.h"
#include "common/error.h"
#include "common/gui_options.h"
#include "common/str.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "common/translation.h"
#include "common/util.h"
#include "common/file.h"
#include "gui/message.h"
#include "audio/mididrv.h"
#include "audio/musicplugin.h"

const byte MidiDriver::_mt32ToGm[128] = {
//	  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	  0,   1,   0,   2,   4,   4,   5,   3,  16,  17,  18,  16,  16,  19,  20,  21, // 0x
	  6,   6,   6,   7,   7,   7,   8, 112,  62,  62,  63,  63,  38,  38,  39,  39, // 1x
	 88,  95,  52,  98,  97,  99,  14,  54, 102,  96,  53, 102,  81, 100,  14,  80, // 2x
	 48,  48,  49,  45,  41,  40,  42,  42,  43,  46,  45,  24,  25,  28,  27, 104, // 3x
	 32,  32,  34,  33,  36,  37,  35,  35,  79,  73,  72,  72,  74,  75,  64,  65, // 4x
	 66,  67,  71,  71,  68,  69,  70,  22,  56,  59,  57,  57,  60,  60,  58,  61, // 5x
	 61,  11,  11,  98,  14,   9,  14,  13,  12, 107, 107,  77,  78,  78,  76,  76, // 6x
	 47, 117, 127, 118, 118, 116, 115, 119, 115, 112,  55, 124, 123,   0,  14, 117  // 7x
};

const byte MidiDriver::_gmToMt32[128] = {
//	  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	  5,   1,   2,   7,   3,   5,  16,  21,  22, 101, 101,  97, 104, 103, 102,  20, // 0x
	  8,   9,  11,  12,  14,  15,  87,  15,  59,  60,  61,  62,  67,  44,  79,  23, // 1x
	 64,  67,  66,  70,  68,  69,  28,  31,  52,  54,  55,  56,  49,  51,  57, 112, // 2x
	 48,  50,  45,  26,  34,  35,  45, 122,  89,  90,  94,  81,  92,  95,  24,  25, // 3x
	 80,  78,  79,  78,  84,  85,  86,  82,  74,  72,  76,  77, 110, 107, 108,  76, // 4x
	 47,  44, 111,  45,  44,  34,  44,  30,  32,  33,  88,  34,  35,  35,  38,  33, // 5x
	 41,  36, 100,  37,  40,  34,  43,  40,  63,  21,  99, 105, 103,  86,  55,  84, // 6x
	101, 103, 100, 120, 117, 113,  99, 128, 128, 128, 128, 124, 123, 128, 128, 128, // 7x
};

// These are the power-on default instruments of the Roland MT-32 family.
const byte MidiDriver::_mt32DefaultInstruments[8] = {
	0x44, 0x30, 0x5F, 0x4E, 0x29, 0x03, 0x6E, 0x7A
};

// These are the power-on default panning settings for channels 2-9 of the Roland MT-32 family.
// Internally, the MT-32 has 15 panning positions (0-E with 7 being center).
// This has been translated to the equivalent MIDI panning values (0-127).
// These are used for setting default panning on GM devices when using them with MT-32 data.
// Note that MT-32 panning is reversed compared to the MIDI specification. This is not reflected
// here; the driver is expected to flip these values based on the _reversePanning variable.
const byte MidiDriver::_mt32DefaultPanning[8] = {
	// 7,    8,    7,    8,    4,    A,    0,    E 
	0x40, 0x49,	0x40, 0x49, 0x25, 0x5B, 0x00, 0x7F
};

// This is the drum map for the Roland Sound Canvas SC-55 v1.xx. It had a fallback mechanism 
// to correct invalid drumkit selections. Some games rely on this mechanism to select the 
// correct Roland GS drumkit. Use this map to emulate this mechanism.
// E.g. correct invalid drumkit 50: _gsDrumkitFallbackMap[50] == 48
const uint8 MidiDriver::_gsDrumkitFallbackMap[128] = {
	 0,  0,  0,  0,  0,  0,  0,  0, // STANDARD
	 8,  8,  8,  8,  8,  8,  8,  8, // ROOM
	16, 16, 16, 16, 16, 16, 16, 16, // POWER
	24, 25, 24, 24, 24, 24, 24, 24, // ELECTRONIC; TR-808 (25)
	32, 32, 32, 32, 32, 32, 32, 32, // JAZZ
	40, 40, 40, 40, 40, 40, 40, 40, // BRUSH
	48, 48, 48, 48, 48, 48, 48, 48, // ORCHESTRA
	56, 56, 56, 56, 56, 56, 56, 56, // SFX
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined (fall back to STANDARD)
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0,  0, // No drumkit defined
	 0,  0,  0,  0,  0,  0,  0, 127 // No drumkit defined; CM-64/32L (127)
};

static const struct {
	uint32      type;
	const char *guio;
} GUIOMapping[] = {
	{ MT_PCSPK,		GUIO_MIDIPCSPK },
	{ MT_CMS,		GUIO_MIDICMS },
	{ MT_PCJR,		GUIO_MIDIPCJR },
	{ MT_ADLIB,		GUIO_MIDIADLIB },
	{ MT_C64,		GUIO_MIDIC64 },
	{ MT_AMIGA,	    GUIO_MIDIAMIGA },
	{ MT_APPLEIIGS,	GUIO_MIDIAPPLEIIGS },
	{ MT_TOWNS,		GUIO_MIDITOWNS },
	{ MT_PC98,		GUIO_MIDIPC98 },
	{ MT_GM,		GUIO_MIDIGM },
	{ MT_MT32,		GUIO_MIDIMT32 },
	{ 0,			0 },
};

Common::String MidiDriver::musicType2GUIO(uint32 musicType) {
	Common::String res;

	for (int i = 0; GUIOMapping[i].guio; i++) {
		if (musicType == GUIOMapping[i].type || musicType == (uint32)-1)
			res += GUIOMapping[i].guio;
	}

	return res;
}

bool MidiDriver::_forceTypeMT32 = false;

MusicType MidiDriver::getMusicType(MidiDriver::DeviceHandle handle) {
	if (_forceTypeMT32)
		return MT_MT32;

	if (handle) {
		const PluginList p = MusicMan.getPlugins();
		for (PluginList::const_iterator m = p.begin(); m != p.end(); m++) {
			MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
			for (MusicDevices::iterator d = i.begin(); d != i.end(); d++) {
				if (handle == d->getHandle())
					return d->getMusicType();
			}
		}
	}

	return MT_INVALID;
}

Common::String MidiDriver::getDeviceString(DeviceHandle handle, DeviceStringType type) {
	if (handle) {
		const PluginList p = MusicMan.getPlugins();
		for (PluginList::const_iterator m = p.begin(); m != p.end(); m++) {
			MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
			for (MusicDevices::iterator d = i.begin(); d != i.end(); d++) {
				if (handle == d->getHandle()) {
					if (type == kDriverName)
						return d->getMusicDriverName();
					else if (type == kDriverId)
						return d->getMusicDriverId();
					else if (type == kDeviceName)
						return d->getCompleteName();
					else if (type == kDeviceId)
						return d->getCompleteId();
					else
						return Common::String("auto");
				}
			}
		}
	}

	return Common::String("auto");
}

MidiDriver::DeviceHandle MidiDriver::detectDevice(int flags) {
	// Query the selected music device (defaults to MT_AUTO device).
	Common::String selDevStr = ConfMan.hasKey("music_driver") ? ConfMan.get("music_driver") : Common::String("auto");
	if ((flags & MDT_PREFER_FLUID) && selDevStr == "auto") {
		selDevStr = "fluidsynth";
	}
	DeviceHandle hdl = getDeviceHandle(selDevStr.empty() ? Common::String("auto") : selDevStr);
	DeviceHandle reslt = 0;

	_forceTypeMT32 = false;

	// Check whether the selected music driver is compatible with the
	// given flags.
	switch (getMusicType(hdl)) {
	case MT_PCSPK:
		if (flags & MDT_PCSPK)
			reslt = hdl;
		break;

	case MT_PCJR:
		if (flags & MDT_PCJR)
			reslt = hdl;
		break;

	case MT_CMS:
		if (flags & MDT_CMS)
			reslt = hdl;
		break;

	case MT_ADLIB:
		if (flags & MDT_ADLIB)
			reslt = hdl;
		break;

	case MT_C64:
		if (flags & MDT_C64)
			reslt = hdl;
		break;

	case MT_AMIGA:
		if (flags & MDT_AMIGA)
			reslt = hdl;
		break;

	case MT_APPLEIIGS:
		if (flags & MDT_APPLEIIGS)
			reslt = hdl;
		break;

	case MT_TOWNS:
		if (flags & MDT_TOWNS)
			reslt = hdl;
		break;

	case MT_PC98:
		if (flags & MDT_PC98)
			reslt = hdl;
		break;

	case MT_GM:
	case MT_GS:
	case MT_MT32:
		if (flags & MDT_MIDI)
			reslt = hdl;
		break;

	case MT_NULL:
		reslt = hdl;
		break;

	default:
		break;
	}

	Common::String failedDevStr;
	if (getMusicType(hdl) == MT_INVALID) {
		// If the expressly selected driver or device cannot be found (no longer compiled in, turned off, etc.)
		// we display a warning and continue.
		failedDevStr = selDevStr;
		Common::String warningMsg = Common::String::format(_("The selected audio device '%s' was not found (e.g. might be turned off or disconnected)."), failedDevStr.c_str()) + " " + _("Attempting to fall back to the next available device...");
		GUI::MessageDialog dialog(warningMsg);
		dialog.runModal();
	}

	MusicType tp = getMusicType(reslt);
	if (tp != MT_INVALID && tp != MT_AUTO) {
		if (checkDevice(reslt)) {
			return reslt;
		} else {
			// If the expressly selected device cannot be used we display a warning and continue.
			failedDevStr = getDeviceString(hdl, MidiDriver::kDeviceName);
			Common::String warningMsg = Common::String::format(_("The selected audio device '%s' cannot be used. See log file for more information."), failedDevStr.c_str()) + " " + _("Attempting to fall back to the next available device...");
			GUI::MessageDialog dialog(warningMsg);
			dialog.runModal();
		}
	}

	// If the selected driver did not match the flags setting,
	// we try to determine a suitable and "optimal" music driver.
	const PluginList p = MusicMan.getPlugins();
	// If only MDT_MIDI but not MDT_PREFER_MT32 or MDT_PREFER_GM is set we prefer the other devices (which will always be
	// detected since they are hard coded and cannot be disabled).
	bool skipMidi = !(flags & (MDT_PREFER_GM | MDT_PREFER_MT32));
	while (flags != MDT_NONE) {
		if ((flags & MDT_MIDI) && !skipMidi) {
			// If a preferred MT32 or GM device has been selected that device gets returned if available.
			Common::String devStr;
			if (flags & MDT_PREFER_MT32)
				devStr = ConfMan.hasKey("mt32_device") ? ConfMan.get("mt32_device") : Common::String("null");
			else if (flags & MDT_PREFER_GM)
				devStr = ConfMan.hasKey("gm_device") ? ConfMan.get("gm_device") : Common::String("null");
			else
				devStr = "auto";

			// Default to Null device here, since we also register a default null setting for
			// the MT32 or GM device in the config manager.
			hdl = getDeviceHandle(devStr.empty() ? Common::String("null") : devStr);
			const MusicType type = getMusicType(hdl);

			// If we have a "Don't use GM/MT-32" setting we skip this part and jump
			// to AdLib, PC Speaker etc. detection right away.
			if (type != MT_NULL) {
				if (type == MT_INVALID) {
					// If the preferred (expressly requested) selected driver or device cannot be found (no longer compiled in, turned off, etc.)
					// we display a warning and continue. Don't warn about the missing device if we did already (this becomes relevant if the
					// missing device is selected as preferred device and also as GM or MT-32 device).
					if (failedDevStr != devStr) {
						Common::String warningMsg = Common::String::format(_("The preferred audio device '%s' was not found (e.g. might be turned off or disconnected)."), devStr.c_str()) + " " + _("Attempting to fall back to the next available device...");
						GUI::MessageDialog dialog(warningMsg);
						dialog.runModal();
					}
				} else if (type != MT_AUTO) {
					if (checkDevice(hdl)) {
						if (flags & MDT_PREFER_MT32)
							// If we have a preferred MT32 device we disable the gm/mt32 mapping (more about this in mididrv.h).
							_forceTypeMT32 = true;
						return hdl;
					} else {
						// If the preferred (expressly requested) device cannot be used we display a warning and continue.
						// Don't warn about the failing device if we did already (this becomes relevant if the failing
						// device is selected as preferred device and also as GM or MT-32 device).
						if (failedDevStr != getDeviceString(hdl, MidiDriver::kDeviceName)) {
							Common::String warningMsg = Common::String::format(_("The preferred audio device '%s' cannot be used. See log file for more information."), getDeviceString(hdl, MidiDriver::kDeviceName).c_str()) + " " + _("Attempting to fall back to the next available device...");
							GUI::MessageDialog dialog(warningMsg);
							dialog.runModal();
						}
					}
				}

				// If no specific device is selected (neither in the scummvm nor in the game domain)
				// and there is no preferred MT32 or GM device selected either or if the detected device is unavailable we arrive here.
				// If MT32 is preferred we try for the first available device with music type 'MT_MT32' (usually the mt32 emulator).
				if (flags & MDT_PREFER_MT32) {
					for (PluginList::const_iterator m = p.begin(); m != p.end(); ++m) {
						MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
						for (MusicDevices::iterator d = i.begin(); d != i.end(); ++d) {
							if (d->getMusicType() == MT_MT32) {
								hdl = d->getHandle();
								if (checkDevice(hdl))
									return hdl;
							}
						}
					}
				}

				// Now we default to the first available device with music type 'MT_GM' if not
				// MT-32 is preferred or if MT-32 is preferred but all other devices have failed.
				if (!(flags & MDT_PREFER_MT32) || flags == (MDT_PREFER_MT32 | MDT_MIDI)) {
					for (PluginList::const_iterator m = p.begin(); m != p.end(); ++m) {
						MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
						for (MusicDevices::iterator d = i.begin(); d != i.end(); ++d) {
							if (d->getMusicType() == MT_GM || d->getMusicType() == MT_GS) {
								hdl = d->getHandle();
								if (checkDevice(hdl))
									return hdl;
							}
						}
					}
					// Detection flags get removed after final detection attempt to avoid further attempts.
					flags &= ~(MDT_MIDI | MDT_PREFER_GM | MDT_PREFER_MT32);
				}
			}
		}

		// The order in this list is important, since this is the order of preference
		// (e.g. MT_ADLIB is checked before MT_PCJR and MT_PCSPK for a good reason).
		// Detection flags get removed after detection attempt to avoid further attempts.
		if (flags & MDT_TOWNS) {
			tp = MT_TOWNS;
			flags &= ~MDT_TOWNS;
		} else if (flags & MDT_PC98) {
			tp = MT_PC98;
			flags &= ~MDT_PC98;
		} else if (flags & MDT_ADLIB) {
			tp = MT_ADLIB;
			flags &= ~MDT_ADLIB;
		} else if (flags & MDT_PCJR) {
			tp = MT_PCJR;
			flags &= ~MDT_PCJR;
		} else if (flags & MDT_PCSPK) {
			tp = MT_PCSPK;
			flags &= ~MDT_PCSPK;
		} else if (flags & MDT_C64) {
			tp = MT_C64;
			flags &= ~MDT_C64;
		} else if (flags & MDT_AMIGA) {
			tp = MT_AMIGA;
			flags &= ~MDT_AMIGA;
		} else if (flags & MDT_APPLEIIGS) {
			tp = MT_APPLEIIGS;
			flags &= ~MDT_APPLEIIGS;
		} else if (flags & MDT_MIDI) {
			// If we haven't tried to find a MIDI device yet we do this now.
			skipMidi = false;
			continue;
		} else if (flags) {
			// Invalid flags. Set them to MDT_NONE to leave detection loop.
			flags = MDT_NONE;
			tp = MT_AUTO;
		}

		for (PluginList::const_iterator m = p.begin(); m != p.end(); ++m) {
			MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
			for (MusicDevices::iterator d = i.begin(); d != i.end(); ++d) {
				if (d->getMusicType() == tp) {
					hdl = d->getHandle();
					if (checkDevice(hdl))
						return hdl;
				}
			}
		}
	}

	return 0;
}

MidiDriver *MidiDriver::createMidi(MidiDriver::DeviceHandle handle) {
	MidiDriver *driver = 0;
	const PluginList p = MusicMan.getPlugins();
	for (PluginList::const_iterator m = p.begin(); m != p.end(); m++) {
		const MusicPluginObject &musicPlugin = (*m)->get<MusicPluginObject>();
		if (getDeviceString(handle, MidiDriver::kDriverId).equals(musicPlugin.getId()))
			musicPlugin.createInstance(&driver, handle);
	}

	return driver;
}

bool MidiDriver::checkDevice(MidiDriver::DeviceHandle handle) {
	const PluginList p = MusicMan.getPlugins();
	for (PluginList::const_iterator m = p.begin(); m != p.end(); m++) {
		const MusicPluginObject &musicPlugin = (*m)->get<MusicPluginObject>();
		if (getDeviceString(handle, MidiDriver::kDriverId).equals(musicPlugin.getId()))
			return musicPlugin.checkDevice(handle);
	}

	return false;
}

MidiDriver::DeviceHandle MidiDriver::getDeviceHandle(const Common::String &identifier) {
	const PluginList p = MusicMan.getPlugins();

	if (p.begin() == p.end())
		error("MidiDriver::getDeviceHandle: Music plugins must be loaded prior to calling this method");

	for (PluginList::const_iterator m = p.begin(); m != p.end(); m++) {
		MusicDevices i = (*m)->get<MusicPluginObject>().getDevices();
		for (MusicDevices::iterator d = i.begin(); d != i.end(); d++) {
			// The music driver id isn't unique, but it will match
			// driver's first device. This is useful when selecting
			// the driver from the command line.
			if (identifier.equals(d->getMusicDriverId()) || identifier.equals(d->getCompleteId()) || identifier.equals(d->getCompleteName())) {
				return d->getHandle();
			}
		}
	}

	return 0;
}

void MidiDriver::initMT32(bool initForGM) {
	sendMT32Reset();

	if (initForGM) {
		// Set up MT-32 for GM data.
		// This is based on Roland's GM settings for MT-32.
		debug("Initializing MT-32 for General MIDI data");

		byte buffer[17];

		// Roland MT-32 SysEx for system area
		memcpy(&buffer[0], "\x41\x10\x16\x12\x10\x00", 6);

		// Set reverb parameters:
		// - Mode 2 (Plate)
		// - Time 3
		// - Level 4
		memcpy(&buffer[6], "\x01\x02\x03\x04\x66", 5);
		sysEx(buffer, 11);

		// Set partial reserve to match SC-55
		memcpy(&buffer[6], "\x04\x08\x04\x04\x03\x03\x03\x03\x02\x02\x4C", 11);
		sysEx(buffer, 17);

		// Use MIDI instrument channels 1-8 instead of 2-9
		memcpy(&buffer[6], "\x0D\x00\x01\x02\x03\x04\x05\x06\x07\x09\x3E", 11);
		sysEx(buffer, 17);

		// The MT-32 has reversed stereo panning compared to the MIDI spec.
		// GM does use panning as specified by the MIDI spec.
		_reversePanning = true;

		int i;

		// Set default GM panning (center on all channels)
		for (i = 0; i < 8; ++i) {
			send((0x40 << 16) | (10 << 8) | (0xB0 | i));
		}

		// Set default GM instruments (0 on all channels).
		// This is expected to be mapped to the MT-32 equivalent by the driver.
		for (i = 0; i < 8; ++i) {
			send((0 << 8) | (0xC0 | i));
		}

		// Set Pitch Bend Sensitivity to 2 semitones.
		for (i = 0; i < 8; ++i) {
			setPitchBendRange(i, 2);
		}
		setPitchBendRange(9, 2);
	}
}

void MidiDriver::sendMT32Reset() {
	static const byte resetSysEx[] = { 0x41, 0x10, 0x16, 0x12, 0x7F, 0x00, 0x00, 0x01, 0x00 };
	sysEx(resetSysEx, sizeof(resetSysEx));
	g_system->delayMillis(100);
}

void MidiDriver::initGM(bool initForMT32, bool enableGS) {
// ResidualVM - not used
}

void MidiDriver::sendGMReset() {
// ResidualVM - not used
}

byte MidiDriver::correctInstrumentBank(byte outputChannel, byte patchId) {
// ResidualVM - not used
		return 0xFF;

}
void MidiDriver_BASE::midiDumpInit() {
// NovelVM - not used
}

int MidiDriver_BASE::midiDumpVarLength(const uint32 &delta) {
// NovelVM - not used
	return 0;
}

void MidiDriver_BASE::midiDumpDelta() {
// NovelVM - not used
}

void MidiDriver_BASE::midiDumpDo(uint32 b) {
// NovelVM - not used
}

void MidiDriver_BASE::midiDumpSysEx(const byte *msg, uint16 length) {
// NovelVM - not used
}


void MidiDriver_BASE::midiDumpFinish() {
// NovelVM - not used
}

MidiDriver_BASE::MidiDriver_BASE() {
// NovelVM - not used
}

MidiDriver_BASE::~MidiDriver_BASE() {
// NovelVM - not used
}

void MidiDriver_BASE::send(byte status, byte firstOp, byte secondOp) {
	send(status | ((uint32)firstOp << 8) | ((uint32)secondOp << 16));
}

void MidiDriver_BASE::send(int8 source, byte status, byte firstOp, byte secondOp) {
	send(source, status | ((uint32)firstOp << 8) | ((uint32)secondOp << 16));
}

void MidiDriver::midiDriverCommonSend(uint32 b) {
// NovelVM - not used
}

void MidiDriver::midiDriverCommonSysEx(const byte *msg, uint16 length) {
// NovelVM - not used
}


