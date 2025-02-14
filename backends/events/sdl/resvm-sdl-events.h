/* NovelVM - A 3D game interpreter
 *
 * NovelVM is the legal property of its developers, whose names
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

#ifndef BACKEND_EVENTS_RESVM_SDL
#define BACKEND_EVENTS_RESVM_SDL

#include "backends/events/sdl/sdl-events.h"

// multiplier used to increase resolution for keyboard/joystick mouse
#define MULTIPLIER 16

/**
 * Custom event source for NovelVM with true joystick support.
 */
class ResVmSdlEventSource : public SdlEventSource {
public:
	ResVmSdlEventSource();

	/**
	 * Resets keyboard emulation after a video screen change
	 */
	void resetKeyboardEmulation(int16 x_max, int16 y_max);

protected:
	struct KbdMouse {
		int32 x, y;
		int16 x_vel, y_vel, x_max, y_max, x_down_count, y_down_count, joy_x, joy_y;
		uint32 last_time, delay_time, x_down_time, y_down_time;
		bool modifier;
	};
	KbdMouse _km;

	bool pollEvent(Common::Event &event) override;
	void updateKbdMouse();

	bool handleMouseMotion(SDL_Event &ev, Common::Event &event) override;
	bool handleMouseButtonDown(SDL_Event &ev, Common::Event &event) override;
	bool handleMouseButtonUp(SDL_Event &ev, Common::Event &event) override;

	bool handleJoyButtonDown(SDL_Event &ev, Common::Event &event) override;
	bool handleJoyButtonUp(SDL_Event &ev, Common::Event &event) override;
	bool handleJoyAxisMotion(SDL_Event &ev, Common::Event &event) override;
	bool handleKbdMouse(Common::Event &event);

	bool processMouseEvent(Common::Event &event, int x, int y, int relx = 0, int rely = 0) override;

	/**
	 * Update the virtual mouse according to a joystick or game controller axis position change
	 */
	virtual bool handleAxisToMouseMotion(int16 xAxis, int16 yAxis);

	/**
	 * Compute the virtual mouse movement speed factor according to the 'kbdmouse_speed' setting.
	 * The speed factor is scaled with the display size.
	 */
	int16 computeJoystickMouseSpeedFactor() const;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	bool handleControllerButton(const SDL_Event &ev, Common::Event &event, bool buttonUp) override;
	bool handleControllerAxisMotion(const SDL_Event &ev, Common::Event &event) override;
#endif

	bool shouldGenerateMouseEvents();
};

#endif
