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

#include "common/scummsys.h"

#if defined(SDL_BACKEND)

#include "backends/graphics/surfacesdl/surfacesdl-graphics.h"
#include "backends/events/sdl/resvm-sdl-events.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "engines/engine.h"
#include "graphics/pixelbuffer.h"
#include "graphics/surface.h"
#ifdef USE_PNG
#include "image/png.h"
#endif

// SDL surface flags which got removed in SDL2.
#if SDL_VERSION_ATLEAST(2, 0, 0)
#define SDL_FULLSCREEN  0x40000000
#endif

SurfaceSdlGraphicsManager::SurfaceSdlGraphicsManager(SdlEventSource *sdlEventSource, SdlWindow *window)
	:
	ResVmSdlGraphicsManager(sdlEventSource, window),
#if SDL_VERSION_ATLEAST(2, 0, 0)
	_renderer(nullptr), _screenTexture(nullptr),
#endif
	_screen(0),
	_subScreen(0),
	_overlayscreen(0),
	_overlayDirty(true),
	_overlayVisible(false),
	_fullscreen(false),
	_lockAspectRatio(true),
	_screenChangeCount(0),
	_gameRect(),
	_engineRequestedWidth(0),
	_engineRequestedHeight(0) {
		ConfMan.registerDefault("aspect_ratio", true);

		_sideSurfaces[0] = _sideSurfaces[1] = nullptr;
}

SurfaceSdlGraphicsManager::~SurfaceSdlGraphicsManager() {
	closeOverlay();

	if (_subScreen) {
		SDL_FreeSurface(_subScreen);
		_subScreen = nullptr;
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)
	deinitializeRenderer();
#endif
}

bool SurfaceSdlGraphicsManager::hasFeature(OSystem::Feature f) const {
	return
#if SDL_VERSION_ATLEAST(2, 0, 0)
		(f == OSystem::kFeatureFullscreenToggleKeepsContext) ||
#endif
		(f == OSystem::kFeatureAspectRatioCorrection) ||
		(f == OSystem::kFeatureFullscreenMode);
}

bool SurfaceSdlGraphicsManager::getFeatureState(OSystem::Feature f) const {
	switch (f) {
		case OSystem::kFeatureFullscreenMode:
			return _fullscreen;
		case OSystem::kFeatureAspectRatioCorrection:
			return _lockAspectRatio;
		default:
			return false;
	}
}

void SurfaceSdlGraphicsManager::setFeatureState(OSystem::Feature f, bool enable) {
	switch (f) {
		case OSystem::kFeatureFullscreenMode:
			if (_fullscreen != enable) {
				_fullscreen = enable;
				createOrUpdateScreen();
			}
			break;
		case OSystem::kFeatureAspectRatioCorrection:
			_lockAspectRatio = enable;
			break;
		default:
			break;
	}
}

void SurfaceSdlGraphicsManager::setupScreen(uint gameWidth, uint gameHeight, bool fullscreen, bool accel3d) {
	assert(!accel3d);

	if (_subScreen) {
		SDL_FreeSurface(_subScreen);
		_subScreen = nullptr;
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)
	deinitializeRenderer();
#endif

	_engineRequestedWidth = gameWidth;
	_engineRequestedHeight = gameHeight;
	_fullscreen = fullscreen;
	_lockAspectRatio = ConfMan.getBool("aspect_ratio");

	createOrUpdateScreen();

	SDL_PixelFormat *f = _screen->format;
	_subScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, gameWidth, gameHeight, f->BitsPerPixel, f->Rmask, f->Gmask, f->Bmask, f->Amask);

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_SetSurfaceBlendMode(_subScreen, SDL_BLENDMODE_NONE);
#endif // SDL_VERSION_ATLEAST(2, 0, 0)
}

void SurfaceSdlGraphicsManager::createOrUpdateScreen() {
	closeOverlay();

	// Choose the effective window size or fullscreen mode
	uint effectiveWidth;
	uint effectiveHeight;
	if (_fullscreen && _lockAspectRatio) {
		Common::Rect fullscreenResolution = getPreferredFullscreenResolution();
		effectiveWidth = fullscreenResolution.width();
		effectiveHeight = fullscreenResolution.height();
	} else {
		effectiveWidth = _engineRequestedWidth;
		effectiveHeight = _engineRequestedHeight;
	}

	// The game is centered inside the effective screen
	_gameRect = Math::Rect2d(
			Math::Vector2d((effectiveWidth - _engineRequestedWidth) / 2, (effectiveHeight - _engineRequestedHeight) / 2),
			Math::Vector2d((effectiveWidth + _engineRequestedWidth) / 2, (effectiveHeight + _engineRequestedHeight) / 2)
	);

	uint32 sdlflags = SDL_SWSURFACE;
	if (_fullscreen)
		sdlflags |= SDL_FULLSCREEN;

	_screen = SDL_SetVideoMode(effectiveWidth, effectiveHeight, ConfMan.getInt("bpp"), sdlflags);
	if (!_screen) {
		warning("SDL_SetVideoMode failed: %s", SDL_GetError());
		g_system->quit();
	}

	SDL_PixelFormat *f = _screen->format;
	_overlayscreen = SDL_CreateRGBSurface(SDL_SWSURFACE, effectiveWidth, effectiveHeight, f->BitsPerPixel,
	                                      f->Rmask, f->Gmask, f->Bmask, f->Amask);

	_overlayFormat = Graphics::PixelFormat(f->BytesPerPixel, 8 - f->Rloss, 8 - f->Gloss, 8 - f->Bloss, 0,
	                                       f->Rshift, f->Gshift, f->Bshift, f->Ashift);

	if (!_overlayscreen) {
		warning("SDL_RGBSurface failed: %s", SDL_GetError());
		g_system->quit();
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_SetSurfaceBlendMode(_overlayscreen, SDL_BLENDMODE_NONE);
#endif // SDL_VERSION_ATLEAST(2, 0, 0)

	_screenFormat = _overlayFormat;

	_screenChangeCount++;

	dynamic_cast<ResVmSdlEventSource *>(_eventSource)->resetKeyboardEmulation(_gameRect.getWidth() - 1, _gameRect.getHeight() - 1);
}

Graphics::PixelBuffer SurfaceSdlGraphicsManager::getScreenPixelBuffer() {
	return Graphics::PixelBuffer(_screenFormat, (byte *)_subScreen->pixels);
}

void SurfaceSdlGraphicsManager::drawSideTextures() {
	if (_fullscreen && _lockAspectRatio) {
		if (_sideSurfaces[0]) {
			SDL_Rect dstrect;
			dstrect.x = _gameRect.getTopLeft().getX() - _sideSurfaces[0]->w;
			dstrect.y = _gameRect.getTopLeft().getY();
			dstrect.w = _sideSurfaces[0]->w;
			dstrect.h = _sideSurfaces[0]->h;
			SDL_BlitSurface(_sideSurfaces[0], NULL, _screen, &dstrect);
		}
		if (_sideSurfaces[1]) {
			SDL_Rect dstrect;
			dstrect.x = _gameRect.getTopRight().getX();
			dstrect.y = _gameRect.getTopLeft().getY();
			dstrect.w = _sideSurfaces[1]->w;
			dstrect.h = _sideSurfaces[1]->h;
			SDL_BlitSurface(_sideSurfaces[1], NULL, _screen, &dstrect);
		}
	}
}

void SurfaceSdlGraphicsManager::drawOverlay() {
	if (!_overlayscreen)
		return;

	SDL_BlitSurface(_overlayscreen, NULL, _screen, NULL);
}

void SurfaceSdlGraphicsManager::updateScreen() {
	SDL_Rect dstrect;
	dstrect.x = _gameRect.getTopLeft().getX();
	dstrect.y = _gameRect.getTopLeft().getY();
	dstrect.w = _gameRect.getWidth();
	dstrect.h = _gameRect.getHeight();
	SDL_BlitSurface(_subScreen, NULL, _screen, &dstrect);

	if (_overlayVisible) {
		drawOverlay();
	}
	drawSideTextures();

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UpdateTexture(_screenTexture, nullptr, _screen->pixels, _screen->pitch);

	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _screenTexture, nullptr, nullptr);
	SDL_RenderPresent(_renderer);
#else
	SDL_Flip(_screen);
#endif
}

int16 SurfaceSdlGraphicsManager::getHeight() const {
	// NovelVM specific
	return _subScreen->h;
}

int16 SurfaceSdlGraphicsManager::getWidth() const {
	// NovelVM specific
	return _subScreen->w;
}

#pragma mark -
#pragma mark --- Overlays ---
#pragma mark -

void SurfaceSdlGraphicsManager::clearOverlay() {
	if (!_overlayscreen)
		return;

	if (!_overlayVisible)
		return;

	SDL_BlitSurface(_screen, NULL, _overlayscreen, NULL);

	_overlayDirty = true;
}

void SurfaceSdlGraphicsManager::suggestSideTextures(Graphics::Surface *left, Graphics::Surface *right) {
	delete _sideSurfaces[0];
	_sideSurfaces[0] = nullptr;
	delete _sideSurfaces[1];
	_sideSurfaces[1] = nullptr;
	if (left) {
		_sideSurfaces[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, left->w, left->h, 32, 0xff << left->format.rShift, 0xff << left->format.gShift, 0xff << left->format.bShift, 0xff << left->format.aShift);
		memcpy(_sideSurfaces[0]->pixels, left->getPixels(), left->w * left->h * 4);
	}
	if (right) {
		_sideSurfaces[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, right->w, right->h, 32, 0xff << right->format.rShift, 0xff << right->format.gShift, 0xff << right->format.bShift, 0xff << right->format.aShift);
		memcpy(_sideSurfaces[1]->pixels, right->getPixels(), right->w * right->h * 4);
	}
}

void SurfaceSdlGraphicsManager::showOverlay() {
	if (_overlayVisible)
		return;

	_overlayVisible = true;

	clearOverlay();

	dynamic_cast<ResVmSdlEventSource *>(_eventSource)->resetKeyboardEmulation(getOverlayWidth() - 1, getOverlayHeight() - 1);
}

void SurfaceSdlGraphicsManager::hideOverlay() {
	if (!_overlayVisible)
		return;

	_overlayVisible = false;

	clearOverlay();

	dynamic_cast<ResVmSdlEventSource *>(_eventSource)->resetKeyboardEmulation(_gameRect.getWidth() - 1, _gameRect.getHeight() - 1);
}

void SurfaceSdlGraphicsManager::grabOverlay(void *buf, int pitch) const {
	if (_overlayscreen == NULL)
		return;

	if (SDL_LockSurface(_overlayscreen) == -1)
		error("SDL_LockSurface failed: %s", SDL_GetError());

	byte *src = (byte *)_overlayscreen->pixels;
	byte *dst = (byte *)buf;
	int h = _overlayscreen->h;
	do {
		memcpy(dst, src, _overlayscreen->w * _overlayscreen->format->BytesPerPixel);
		src += _overlayscreen->pitch;
		dst += pitch;
	} while (--h);

	SDL_UnlockSurface(_overlayscreen);
}

void SurfaceSdlGraphicsManager::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	if (_overlayscreen == NULL)
		return;

	const byte *src = (const byte *)buf;

	// Clip the coordinates
	if (x < 0) {
		w += x;
		src -= x * _overlayscreen->format->BytesPerPixel;
		x = 0;
	}

	if (y < 0) {
		h += y;
		src -= y * pitch;
		y = 0;
	}

	if (w > _overlayscreen->w - x) {
		w = _overlayscreen->w - x;
	}

	if (h > _overlayscreen->h - y) {
		h = _overlayscreen->h - y;
	}

	if (w <= 0 || h <= 0)
		return;

	if (SDL_LockSurface(_overlayscreen) == -1)
		error("SDL_LockSurface failed: %s", SDL_GetError());

	byte *dst = (byte *)_overlayscreen->pixels + y * _overlayscreen->pitch + x * _overlayscreen->format->BytesPerPixel;
	do {
		memcpy(dst, src, w * _overlayscreen->format->BytesPerPixel);
		dst += _overlayscreen->pitch;
		src += pitch;
	} while (--h);

	SDL_UnlockSurface(_overlayscreen);
}

void SurfaceSdlGraphicsManager::closeOverlay() {
	SDL_FreeSurface(_sideSurfaces[0]);
	SDL_FreeSurface(_sideSurfaces[1]);
	_sideSurfaces[0] = _sideSurfaces[1] = nullptr;
	if (_overlayscreen) {
		SDL_FreeSurface(_overlayscreen);
		_overlayscreen = nullptr;
	}
	if (_screen) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_FreeSurface(_screen);
#endif
		_screen = nullptr;
	}
}

void SurfaceSdlGraphicsManager::warpMouse(int x, int y) {
	//NovelVM specific
	// Scale from game coordinates to screen coordinates
	x = (x * _gameRect.getWidth()) / _subScreen->w;
	y = (y * _gameRect.getHeight()) / _subScreen->h;

	x += _gameRect.getTopLeft().getX();
	y += _gameRect.getTopLeft().getY();

	_window->warpMouseInWindow(x, y);
}

void SurfaceSdlGraphicsManager::transformMouseCoordinates(Common::Point &point) {
	if (_overlayVisible)
		return;

	// Scale from screen coordinates to game coordinates
	point.x -= _gameRect.getTopLeft().getX();
	point.y -= _gameRect.getTopLeft().getY();

	point.x = (point.x * _subScreen->w) / _gameRect.getWidth();
	point.y = (point.y * _subScreen->h) / _gameRect.getHeight();

	// Make sure we only supply valid coordinates.
	point.x = CLIP<int16>(point.x, 0, _subScreen->w - 1);
	point.y = CLIP<int16>(point.y, 0, _subScreen->h - 1);
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
void SurfaceSdlGraphicsManager::deinitializeRenderer() {
	SDL_DestroyTexture(_screenTexture);
	_screenTexture = nullptr;

	SDL_DestroyRenderer(_renderer);
	_renderer = nullptr;
}

SDL_Surface *SurfaceSdlGraphicsManager::SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
	deinitializeRenderer();

	uint32 createWindowFlags = 0;
	Uint32 rmask, gmask, bmask, amask, format;
	int depth;
#ifdef USE_SDL_RESIZABLE_WINDOW
	createWindowFlags |= SDL_WINDOW_RESIZABLE;
#endif
	if ((flags & SDL_FULLSCREEN) != 0) {
		// On Linux/X11, when toggling to fullscreen, the window manager saves
		// the window size to be able to restore it when going back to windowed mode.
		// If the user configured NovelVM to start in fullscreen mode, we first
		// create a window and then toggle it to fullscreen to give the window manager
		// a chance to save the window size. That way if the user switches back
		// to windowed mode, the window manager has a window size to apply instead
		// of leaving the window at the fullscreen resolution size.
		if (!_window->getSDLWindow()) {
			_window->createOrUpdateWindow(width, height, createWindowFlags);
		}

		createWindowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	if (!_window->createOrUpdateWindow(width, height, createWindowFlags)) {
		return nullptr;
	}

	_renderer = SDL_CreateRenderer(_window->getSDLWindow(), -1, 0);
	if (!_renderer) {
		deinitializeRenderer();
		return nullptr;
	}

	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);

	switch (bpp) {
	case 0:
	case 32:
		format = SDL_PIXELFORMAT_ARGB8888;
		break;
	case 16:
		format = SDL_PIXELFORMAT_RGB565;
		break;
	default:
		warning("Unsupported bpp value: %i", bpp);
		deinitializeRenderer();
		return nullptr;
	}

	_screenTexture = SDL_CreateTexture(_renderer, format, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!_screenTexture) {
		deinitializeRenderer();
		return nullptr;
	}

	if (!SDL_PixelFormatEnumToMasks(format, &depth, &rmask, &gmask, &bmask, &amask)) {
		deinitializeRenderer();
		return nullptr;
	}

	SDL_Surface *screen = SDL_CreateRGBSurface(0, width, height, depth, rmask, gmask, bmask, amask);
	if (!screen) {
		deinitializeRenderer();
		return nullptr;
	} else {
		SDL_SetSurfaceBlendMode(screen, SDL_BLENDMODE_NONE);
		return screen;
	}
}
#endif // SDL_VERSION_ATLEAST(2, 0, 0)

bool SurfaceSdlGraphicsManager::saveScreenshot(const Common::String &filename) const {
	// Based on the implementation from ScummVM
	bool success;
	SDL_Surface *screen = nullptr;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	int width, height;
	SDL_GetRendererOutputSize(_renderer, &width, &height);

	screen = SDL_CreateRGBSurface(SDL_SWSURFACE,
								  width,
								  height,
								  24,
#ifdef SCUMM_LITTLE_ENDIAN
								  0x0000FF, 0x00FF00, 0xFF0000,
#else
								  0xFF0000, 0x00FF00, 0x0000FF,
#endif // SCUMM_LITTLE_ENDIAN
								  0);

	SDL_RenderReadPixels(_renderer, nullptr, SDL_PIXELFORMAT_RGB24, screen->pixels, screen->pitch);
#else
	screen = _screen;
#endif // SDL_VERSION_ATLEAST(2, 0, 0)

#ifdef USE_PNG
	Common::DumpFile out;
	if (!out.open(filename)) {
		success = false;
	} else {
		if (SDL_LockSurface(screen) < 0) {
			warning("Could not lock the screen surface");
			success = false;
		}

#ifdef SCUMM_LITTLE_ENDIAN
		const Graphics::PixelFormat format(3, 8, 8, 8, 0, 0, 8, 16, 0);
#else
		const Graphics::PixelFormat format(3, 8, 8, 8, 0, 16, 8, 0, 0);
#endif
		Graphics::Surface data;
		data.init(screen->w, screen->h, screen->pitch, screen->pixels, format);
		success = Image::writePNG(out, data);

		SDL_UnlockSurface(screen);
	}
#else
	success = SDL_SaveBMP(screen, filename.c_str()) == 0;
#endif

	if (screen && screen != _screen) {
		SDL_FreeSurface(screen);
	}

	return success;
}

int SurfaceSdlGraphicsManager::getGraphicsModeScale(int mode) const {
	return 1;
}

#endif
