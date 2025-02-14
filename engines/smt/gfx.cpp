/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
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

#if defined(WIN32)
#include <windows.h>
// winnt.h defines ARRAYSIZE, but we want our own one...
#undef ARRAYSIZE
#endif

#include "engines/smt/gfx.h"

#include "common/config-manager.h"

#include "graphics/renderer.h"
#include "graphics/surface.h"

#ifdef USE_OPENGL
#include "graphics/opengl/context.h"
#endif

#include "math/glmath.h"

namespace SMT {

const float Renderer::cubeVertices[] = {
	// S     T      X      Y      Z
	0.0f, 1.0f, -320.0f, -320.0f, -320.0f,
	1.0f, 1.0f,  320.0f, -320.0f, -320.0f,
	0.0f, 0.0f, -320.0f,  320.0f, -320.0f,
	1.0f, 0.0f,  320.0f,  320.0f, -320.0f,
	0.0f, 1.0f,  320.0f, -320.0f, -320.0f,
	1.0f, 1.0f, -320.0f, -320.0f, -320.0f,
	0.0f, 0.0f,  320.0f, -320.0f,  320.0f,
	1.0f, 0.0f, -320.0f, -320.0f,  320.0f,
	0.0f, 1.0f,  320.0f, -320.0f,  320.0f,
	1.0f, 1.0f, -320.0f, -320.0f,  320.0f,
	0.0f, 0.0f,  320.0f,  320.0f,  320.0f,
	1.0f, 0.0f, -320.0f,  320.0f,  320.0f,
	0.0f, 1.0f,  320.0f, -320.0f, -320.0f,
	1.0f, 1.0f,  320.0f, -320.0f,  320.0f,
	0.0f, 0.0f,  320.0f,  320.0f, -320.0f,
	1.0f, 0.0f,  320.0f,  320.0f,  320.0f,
	0.0f, 1.0f, -320.0f, -320.0f,  320.0f,
	1.0f, 1.0f, -320.0f, -320.0f, -320.0f,
	0.0f, 0.0f, -320.0f,  320.0f,  320.0f,
	1.0f, 0.0f, -320.0f,  320.0f, -320.0f,
	0.0f, 1.0f,  320.0f,  320.0f,  320.0f,
	1.0f, 1.0f, -320.0f,  320.0f,  320.0f,
	0.0f, 0.0f,  320.0f,  320.0f, -320.0f,
	1.0f, 0.0f, -320.0f,  320.0f, -320.0f
};

Renderer::Renderer(OSystem *system)
		: _system(system),
		  _font(nullptr) {

	// Compute the cube faces Axis Aligned Bounding Boxes
	for (uint i = 0; i < ARRAYSIZE(_cubeFacesAABB); i++) {
		for (uint j = 0; j < 4; j++) {
			_cubeFacesAABB[i].expand(Math::Vector3d(cubeVertices[5 * (4 * i + j) + 2], cubeVertices[5 * (4 * i + j) + 3], cubeVertices[5 * (4 * i + j) + 4]));
		}
	}
}

Renderer::~Renderer() {
}

void Renderer::initFont(const Graphics::Surface *surface) {
	_font = createTexture(surface);
}

void Renderer::freeFont() {
	if (_font) {
		freeTexture(_font);
		_font = nullptr;
	}
}

Texture *Renderer::copyScreenshotToTexture() {
	Graphics::Surface *surface = getScreenshot();

	Texture *texture = createTexture(surface);

	surface->free();
	delete surface;

	return texture;
}

Common::Rect Renderer::getFontCharacterRect(uint8 character) {
	uint index = 0;

	if (character == ' ')
		index = 0;
	else if (character >= '0' && character <= '9')
		index = 1 + character - '0';
	else if (character >= 'A' && character <= 'Z')
		index = 1 + 10 + character - 'A';
	else if (character == '|')
		index = 1 + 10 + 26;
	else if (character == '/')
		index = 2 + 10 + 26;
	else if (character == ':')
		index = 3 + 10 + 26;

	return Common::Rect(16 * index, 0, 16 * (index + 1), 32);
}

Common::Rect Renderer::viewport() const {
	return _screenViewport;
}

void Renderer::computeScreenViewport() {
	int32 screenWidth = _system->getWidth();
	int32 screenHeight = _system->getHeight();

	_screenViewport = Common::Rect(screenWidth, screenHeight);
}

Math::Matrix4 Renderer::makeProjectionMatrix(float fov) const {
	static const float nearClipPlane = 1.0;
	static const float farClipPlane = 10000.0;

	float aspectRatio = 16/9;

	float xmaxValue = nearClipPlane * tan(fov * M_PI / 360.0);
	float ymaxValue = xmaxValue / aspectRatio;

	return Math::makeFrustumMatrix(-xmaxValue, xmaxValue, -ymaxValue, ymaxValue, nearClipPlane, farClipPlane);
}

void Renderer::setupCameraPerspective(float pitch, float heading, float fov) {
	_projectionMatrix = makeProjectionMatrix(fov);
	_modelViewMatrix = Math::Matrix4(180.0f - heading, pitch, 0.0f, Math::EO_YXZ);

	Math::Matrix4 proj = _projectionMatrix;
	Math::Matrix4 model = _modelViewMatrix;
	proj.transpose();
	model.transpose();

	_mvpMatrix = proj * model;

	_frustum.setup(_mvpMatrix);

	_mvpMatrix.transpose();
}

bool Renderer::isCubeFaceVisible(uint face) {
	assert(face < 6);

	return _frustum.isInside(_cubeFacesAABB[face]);
}

void Renderer::flipVertical(Graphics::Surface *s) {
	for (int y = 0; y < s->h / 2; ++y) {
		// Flip the lines
		byte *line1P = (byte *)s->getBasePtr(0, y);
		byte *line2P = (byte *)s->getBasePtr(0, s->h - y - 1);

		for (int x = 0; x < s->pitch; ++x)
			SWAP(line1P[x], line2P[x]);
	}
}

Renderer *createRenderer(OSystem *system) {
	Common::String rendererConfig = ConfMan.get("renderer");
	Graphics::RendererType desiredRendererType = Graphics::parseRendererTypeCode(rendererConfig);
	Graphics::RendererType matchingRendererType = Graphics::getBestMatchingAvailableRendererType(desiredRendererType);

	bool fullscreen = ConfMan.getBool("fullscreen");
	bool isAccelerated = matchingRendererType != Graphics::kRendererTypeTinyGL;

	uint width = system->getWidth();
	uint height = system->getHeight();
	system->setupScreen(width, height, fullscreen, isAccelerated);

#if defined(USE_OPENGL)
	// Check the OpenGL context actually supports shaders
	if (matchingRendererType == Graphics::kRendererTypeOpenGLShaders && !OpenGLContext.shadersSupported) {
		matchingRendererType = Graphics::kRendererTypeOpenGL;
	}
#endif

	if (matchingRendererType != desiredRendererType && desiredRendererType != Graphics::kRendererTypeDefault) {
		// Display a warning if unable to use the desired renderer
		warning("Unable to create a '%s' renderer", rendererConfig.c_str());
	}

#if defined(USE_GLES2) || defined(USE_OPENGL_SHADERS)
	if (matchingRendererType == Graphics::kRendererTypeOpenGLShaders) {
		return CreateGfxOpenGLShader(system);
	}
#endif
#if defined(USE_OPENGL) && !defined(USE_GLES2)
	if (matchingRendererType == Graphics::kRendererTypeOpenGL) {
		return CreateGfxOpenGL(system);
	}
#endif
	if (matchingRendererType == Graphics::kRendererTypeTinyGL) {
		return CreateGfxTinyGL(system);
	}

	error("Unable to create a '%s' renderer", rendererConfig.c_str());
}

void Renderer::toggleFullscreen() {
	if (!_system->hasFeature(OSystem::kFeatureFullscreenToggleKeepsContext)) {
		warning("Unable to toggle the fullscreen state because the current backend would destroy the graphics context");
		return;
	}

	bool oldFullscreen = _system->getFeatureState(OSystem::kFeatureFullscreenMode);
	_system->setFeatureState(OSystem::kFeatureFullscreenMode, !oldFullscreen);
}

void Renderer::renderDrawable(Drawable *drawable, Window *window) {
	if (drawable->isConstrainedToWindow()) {
		selectTargetWindow(window, drawable->is3D(), drawable->isScaled());
	} else {
		selectTargetWindow(nullptr, drawable->is3D(), drawable->isScaled());
	}
	drawable->draw();
}

void Renderer::renderDrawableOverlay(Drawable *drawable, Window *window) {
	// Overlays are always 2D
	if (drawable->isConstrainedToWindow()) {
		selectTargetWindow(window, drawable->is3D(), drawable->isScaled());
	} else {
		selectTargetWindow(nullptr, drawable->is3D(), drawable->isScaled());
	}
	drawable->drawOverlay();
}

void Renderer::renderWindow(Window *window) {
	renderDrawable(window, window);
}

void Renderer::renderWindowOverlay(Window *window) {
	renderDrawableOverlay(window, window);
}

Drawable::Drawable() :
		_isConstrainedToWindow(true),
		_is3D(false),
		_scaled(true) {
}

Common::Point Window::getCenter() const {
	Common::Rect frame = getPosition();

	return Common::Point((frame.left + frame.right) / 2, (frame.top + frame.bottom) / 2);
}

Common::Point Window::screenPosToWindowPos(const Common::Point &screen) const {
	Common::Rect frame = getPosition();

	return Common::Point(screen.x - frame.left, screen.y - frame.top);
}

Common::Point Window::scalePoint(const Common::Point &screen) const {
	Common::Rect viewport = getPosition();
	Common::Rect originalViewport = getOriginalPosition();

	Common::Point scaledPosition = screen;
	scaledPosition.x -= viewport.left;
	scaledPosition.y -= viewport.top;
	scaledPosition.x = CLIP<int16>(scaledPosition.x, 0, viewport.width());
	scaledPosition.y = CLIP<int16>(scaledPosition.y, 0, viewport.height());

	if (_scaled) {
		scaledPosition.x *= originalViewport.width() / (float) viewport.width();
		scaledPosition.y *= originalViewport.height() / (float) viewport.height();
	}

	return scaledPosition;
}

FrameLimiter::FrameLimiter(OSystem *system, const uint framerate) :
	_system(system),
	_speedLimitMs(0),
	_startFrameTime(0) {
	// The frame limiter is disabled when vsync is enabled.
	_enabled = !_system->getFeatureState(OSystem::kFeatureVSync) && framerate != 0;

	if (_enabled) {
		_speedLimitMs = 1000 / CLIP<uint>(framerate, 0, 100);
	}
}

void FrameLimiter::startFrame() {
	_startFrameTime = _system->getMillis();
}

void FrameLimiter::delayBeforeSwap() {
	uint endFrameTime = _system->getMillis();
	uint frameDuration = endFrameTime - _startFrameTime;

	if (_enabled && frameDuration < _speedLimitMs) {
		_system->delayMillis(_speedLimitMs - frameDuration);
	}
}

const Graphics::PixelFormat Texture::getRGBAPixelFormat() {
#ifdef SCUMM_BIG_ENDIAN
	return Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
#else
	return Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
#endif
}
} // End of namespace SMT
