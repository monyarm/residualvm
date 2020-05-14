# This file contains port specific Makefile rules. It is automatically
# included by the default (main) Makefile.
#

#
# POSIX specific
#
install:
	$(INSTALL) -d "$(DESTDIR)$(bindir)"
	$(INSTALL) -c -m 755 "./$(EXECUTABLE)" "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	$(INSTALL) -d "$(DESTDIR)$(mandir)/man6/"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.6" "$(DESTDIR)$(mandir)/man6/novelvm.6"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/pixmaps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/novelvm.xpm" "$(DESTDIR)$(datarootdir)/pixmaps/novelvm.xpm"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/novelvm.svg" "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/novelvm.svg"
	$(INSTALL) -d "$(DESTDIR)$(docdir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_DOCS) "$(DESTDIR)$(docdir)"
	$(INSTALL) -d "$(DESTDIR)$(datadir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_THEMES) $(DIST_FILES_NETWORKING) $(DIST_FILES_VKEYBD) $(DIST_FILES_ENGINEDATA) "$(DESTDIR)$(datadir)/"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/applications"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.desktop" "$(DESTDIR)$(datarootdir)/applications/novelvm.desktop"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/appdata"
#	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.appdata.xml" "$(DESTDIR)$(datarootdir)/appdata/novelvm.appdata.xml"
	# NovelVM specific
ifdef USE_OPENGL_SHADERS
	$(INSTALL) -d "$(DESTDIR)$(datadir)/shaders"
	$(INSTALL) -c -m 644 $(DIST_FILES_SHADERS) "$(DESTDIR)$(datadir)/shaders"
endif
ifdef DYNAMIC_MODULES
	$(INSTALL) -d "$(DESTDIR)$(libdir)/novelvm/"
	$(INSTALL) -c -m 644 $(PLUGINS) "$(DESTDIR)$(libdir)/novelvm/"
endif

install-strip:
	$(INSTALL) -d "$(DESTDIR)$(bindir)"
	$(INSTALL) -c -s -m 755 "./$(EXECUTABLE)" "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	$(INSTALL) -d "$(DESTDIR)$(mandir)/man6/"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.6" "$(DESTDIR)$(mandir)/man6/novelvm.6"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/pixmaps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/novelvm.xpm" "$(DESTDIR)$(datarootdir)/pixmaps/novelvm.xpm"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/novelvm.svg" "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/novelvm.svg"
	$(INSTALL) -d "$(DESTDIR)$(docdir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_DOCS) "$(DESTDIR)$(docdir)"
	$(INSTALL) -d "$(DESTDIR)$(datadir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_THEMES) $(DIST_FILES_NETWORKING) $(DIST_FILES_VKEYBD) $(DIST_FILES_ENGINEDATA) "$(DESTDIR)$(datadir)/"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/applications"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.desktop" "$(DESTDIR)$(datarootdir)/applications/novelvm.desktop"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/appdata"
#	$(INSTALL) -c -m 644 "$(srcdir)/dists/novelvm.appdata.xml" "$(DESTDIR)$(datarootdir)/appdata/novelvm.appdata.xml"
	# NovelVM specific
ifdef USE_OPENGL_SHADERS
	$(INSTALL) -d "$(DESTDIR)$(datadir)/shaders"
	$(INSTALL) -c -m 644 $(DIST_FILES_SHADERS) "$(DESTDIR)$(datadir)/shaders"
endif
ifdef DYNAMIC_MODULES
	$(INSTALL) -d "$(DESTDIR)$(libdir)/novelvm/"
	$(INSTALL) -c -s -m 644 $(PLUGINS) "$(DESTDIR)$(libdir)/novelvm/"
endif

uninstall:
	rm -f "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	rm -f "$(DESTDIR)$(mandir)/man6/novelvm.6"
	rm -f "$(DESTDIR)$(datarootdir)/pixmaps/novelvm.xpm"
	rm -f "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/novelvm.svg"
	rm -rf "$(DESTDIR)$(docdir)"
	rm -rf "$(DESTDIR)$(datadir)"
	rm -f "$(DESTDIR)$(datarootdir)/applications/scummvm.desktop"
	rm -f "$(DESTDIR)$(datarootdir)/appdata/scummvm.appdata.xml"
ifdef DYNAMIC_MODULES
	rm -rf "$(DESTDIR)$(libdir)/novelvm/"
endif

#NovelVM specific:
deb:
	ln -sf dists/debian;
	debian/prepare
	fakeroot debian/rules binary

# Special target to create a application wrapper for Mac OS X

ifdef USE_DOCKTILEPLUGIN

# The NsDockTilePlugIn needs to be compiled in both 32 and 64 bits irrespective of how ScummVM itself is compiled.
# Therefore do not use $(CXXFLAGS) and $(LDFLAGS).

ScummVMDockTilePlugin32.o:
	$(CXX) -mmacosx-version-min=10.6 -arch i386 -O2 -c $(srcdir)/backends/taskbar/macosx/dockplugin/dockplugin.m -o ScummVMDockTilePlugin32.o

ScummVMDockTilePlugin32: ScummVMDockTilePlugin32.o
	$(CXX) -mmacosx-version-min=10.6 -arch i386 -bundle -framework Foundation -framework AppKit -fobjc-link-runtime ScummVMDockTilePlugin32.o -o ScummVMDockTilePlugin32

ScummVMDockTilePlugin64.o:
	$(CXX) -mmacosx-version-min=10.6 -arch x86_64 -O2 -c $(srcdir)/backends/taskbar/macosx/dockplugin/dockplugin.m -o ScummVMDockTilePlugin64.o

ScummVMDockTilePlugin64: ScummVMDockTilePlugin64.o
	$(CXX) -mmacosx-version-min=10.6 -arch x86_64 -bundle -framework Foundation -framework AppKit -fobjc-link-runtime ScummVMDockTilePlugin64.o -o ScummVMDockTilePlugin64

ifdef MACOSX_64_BITS_ONLY
ScummVMDockTilePlugin: ScummVMDockTilePlugin64
	cp ScummVMDockTilePlugin64 ScummVMDockTilePlugin
else
ScummVMDockTilePlugin: ScummVMDockTilePlugin32 ScummVMDockTilePlugin64
	lipo -create ScummVMDockTilePlugin32 ScummVMDockTilePlugin64 -output NovelVMDockTilePlugin
endif

novelvm.docktileplugin: NovelVMDockTilePlugin
	mkdir -p novelvm.docktileplugin/Contents
	cp $(srcdir)/dists/macosx/dockplugin/Info.plist novelvm.docktileplugin/Contents
	mkdir -p novelvm.docktileplugin/Contents/MacOS
	cp NovelVMDockTilePlugin novelvm.docktileplugin/Contents/MacOS/
	chmod 644 novelvm.docktileplugin/Contents/MacOS/NovelVMDockTilePlugin

endif

bundle_name = NovelVM.app

bundle-pack:
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	sed -e 's/$$(PRODUCT_BUNDLE_IDENTIFIER)/org.novelvm.novelvm/' $(srcdir)/dists/macosx/Info.plist >$(bundle_name)/Contents/Info.plist
ifdef USE_SPARKLE
	mkdir -p $(bundle_name)/Contents/Frameworks
	cp $(srcdir)/dists/macosx/dsa_pub.pem $(bundle_name)/Contents/Resources/
	rm -rf $(bundle_name)/Contents/Frameworks/Sparkle.framework
	cp -R $(SPARKLEPATH)/Sparkle.framework $(bundle_name)/Contents/Frameworks/
endif
ifdef MACOSX_USE_LEGACY_ICONS
	cp $(srcdir)/icons/novelvm.icns $(bundle_name)/Contents/Resources/
else
	cp $(srcdir)/icons/novelvm.icns $(bundle_name)/Contents/Resources/
endif
	cp $(DIST_FILES_DOCS) $(bundle_name)/Contents/Resources/
	cp $(DIST_FILES_THEMES) $(bundle_name)/Contents/Resources/
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(bundle_name)/Contents/Resources/
endif
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(bundle_name)/Contents/Resources/
endif
ifdef DIST_FILES_VKEYBD
	cp $(DIST_FILES_VKEYBD) $(bundle_name)/Contents/Resources/
endif
ifdef USE_OPENGL_SHADERS
	mkdir -p $(bundle_name)/Contents/Resources/shaders
	cp $(DIST_FILES_SHADERS) $(bundle_name)/Contents/Resources/shaders/
endif
	$(srcdir)/devtools/credits.pl --rtf > $(bundle_name)/Contents/Resources/AUTHORS.rtf
	rm $(bundle_name)/Contents/Resources/AUTHORS
	mv $(bundle_name)/Contents/Resources/README.md $(bundle_name)/Contents/Resources/README # NovelVM
	cp $(bundle_name)/Contents/Resources/COPYING.LGPL $(bundle_name)/Contents/Resources/COPYING-LGPL
	cp $(bundle_name)/Contents/Resources/COPYING.FREEFONT $(bundle_name)/Contents/Resources/COPYING-FREEFONT
	cp $(bundle_name)/Contents/Resources/COPYING.OFL $(bundle_name)/Contents/Resources/COPYING-OFL
	cp $(bundle_name)/Contents/Resources/COPYING.BSD $(bundle_name)/Contents/Resources/COPYING-BSD
	chmod 644 $(bundle_name)/Contents/Resources/*
ifdef USE_OPENGL_SHADERS
	chmod 755 $(bundle_name)/Contents/Resources/shaders
endif
	cp novelvm-static $(bundle_name)/Contents/MacOS/novelvm
	chmod 755 $(bundle_name)/Contents/MacOS/novelvm
	$(STRIP) $(bundle_name)/Contents/MacOS/novelvm
ifdef USE_DOCKTILEPLUGIN
	mkdir -p $(bundle_name)/Contents/PlugIns
	cp -r novelvm.docktileplugin $(bundle_name)/Contents/PlugIns/
endif

ifdef USE_DOCKTILEPLUGIN
bundle: novelvm-static novelvm.docktileplugin bundle-pack
else
bundle: novelvm-static bundle-pack
endif

iphonebundle: iphone
	mkdir -p $(bundle_name)
	cp $(srcdir)/dists/iphone/Info.plist $(bundle_name)/
	cp $(DIST_FILES_DOCS) $(bundle_name)/
	cp $(DIST_FILES_THEMES) $(bundle_name)/
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(bundle_name)/
endif
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(bundle_name)/
endif
ifdef DIST_FILES_VKEYBD
	cp $(DIST_FILES_VKEYBD) $(bundle_name)/
endif
	$(STRIP) novelvm
	ldid -S novelvm
	chmod 755 novelvm
	cp novelvm $(bundle_name)/NovelVM
	cp $(srcdir)/dists/iphone/icon.png $(bundle_name)/
	cp $(srcdir)/dists/iphone/icon-72.png $(bundle_name)/
	cp $(srcdir)/dists/iphone/Default.png $(bundle_name)/

# Location of static libs for the iPhone
ifneq ($(BACKEND), iphone)
ifneq ($(BACKEND), ios7)
# Static libaries, used for the scummvm-static and iphone targets
OSX_STATIC_LIBS := `$(SDLCONFIG) --prefix=$(STATICLIBPATH) --static-libs`
ifdef USE_SDL_NET
ifdef USE_SDL2
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libSDL2_net.a
else
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libSDL_net.a
endif
endif
# With sdl2-config we don't always get the OpenGL framework
OSX_STATIC_LIBS += -framework OpenGL
endif
endif

ifdef USE_LIBCURL
OSX_STATIC_LIBS += -lcurl
endif

ifdef USE_FREETYPE2
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libfreetype.a $(STATICLIBPATH)/lib/libbz2.a
endif

ifdef USE_FRIBIDI
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libfribidi.a
endif

ifdef USE_ICONV
OSX_STATIC_LIBS += -liconv
endif

ifdef USE_VORBIS
OSX_STATIC_LIBS += \
		$(STATICLIBPATH)/lib/libvorbisfile.a \
		$(STATICLIBPATH)/lib/libvorbis.a
endif

ifdef USE_TREMOR
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libvorbisidec.a
endif

ifdef USE_FLAC
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libFLAC.a
endif

ifdef USE_OGG
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libogg.a
endif

ifdef USE_FLUIDSYNTH
# If iconv was not yet added, add it now as we need it for libfluidsynth
ifndef USE_ICONV
OSX_STATIC_LIBS += -liconv
endif
OSX_STATIC_LIBS += \
                -framework CoreMIDI -framework CoreAudio\
                $(STATICLIBPATH)/lib/libfluidsynth.a \
                $(STATICLIBPATH)/lib/libglib-2.0.a \
                $(STATICLIBPATH)/lib/libintl.a

ifneq ($(BACKEND), iphone)
ifneq ($(BACKEND), ios7)
OSX_STATIC_LIBS += -lreadline -framework AudioUnit
endif
endif
endif

ifdef USE_MAD
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libmad.a
endif

ifdef USE_PNG
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libpng.a
endif

ifdef USE_THEORADEC
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libtheoradec.a
endif

ifdef USE_FAAD
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libfaad.a
endif

ifdef USE_MPEG2
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libmpeg2.a
endif

ifdef USE_A52
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/liba52.a
endif

ifdef USE_JPEG
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libjpeg.a
endif

ifdef USE_ZLIB
OSX_ZLIB ?= $(STATICLIBPATH)/lib/libz.a
endif

ifdef USE_SPARKLE
ifdef MACOSX
ifneq ($(SPARKLEPATH),)
OSX_STATIC_LIBS += -F$(SPARKLEPATH)
endif
OSX_STATIC_LIBS += -framework Sparkle -Wl,-rpath,@loader_path/../Frameworks
endif
endif

# NovelVM specific:
ifdef USE_GLEW
OSX_STATIC_LIBS += $(STATICLIBPATH)/lib/libglew.a
endif
# Special target to create a static linked binary for Mac OS X.
# We use -force_cpusubtype_ALL to ensure the binary runs on every
# PowerPC machine.
novelvm-static: $(OBJS)
	$(CXX) $(LDFLAGS) -force_cpusubtype_ALL -o novelvm-static $(OBJS) \
		-framework CoreMIDI \
		$(OSX_STATIC_LIBS) \
		$(OSX_ZLIB)

# Special target to create a static linked binary for the iPhone (legacy, and iOS 7+)
iphone: $(OBJS)
	$(CXX) $(LDFLAGS) -o novelvm $(OBJS) \
		$(OSX_STATIC_LIBS) \
		-framework UIKit -framework CoreGraphics -framework OpenGLES \
		-framework CoreFoundation -framework QuartzCore -framework Foundation \
		-framework AudioToolbox -framework CoreAudio -framework SystemConfiguration -lobjc -lz

# Special target to create a snapshot disk image for Mac OS X
# TODO: Replace AUTHORS by Credits.rtf
osxsnap: bundle
	mkdir NovelVM-snapshot
	cp $(DIST_FILES_DOCS) ./NovelVM-snapshot/
	mv ./NovelVM-snapshot/COPYING ./NovelVM-snapshot/License\ \(GPL\)
	mv ./NovelVM-snapshot/COPYING.LGPL ./NovelVM-snapshot/License\ \(LGPL\)
	mv ./NovelVM-snapshot/COPYING.FREEFONT ./NovelVM-snapshot/License\ \(FREEFONT\)
	mv ./NovelVM-snapshot/COPYING.OFL ./NovelVM-snapshot/License\ \(OFL\)
	mv ./NovelVM-snapshot/COPYING.BSD ./NovelVM-snapshot/License\ \(BSD\)
	mv ./NovelVM-snapshot/COPYING.ISC ./NovelVM-snapshot/License\ \(ISC\)
	mv ./NovelVM-snapshot/COPYING.LUA ./NovelVM-snapshot/License\ \(Lua\)
	mv ./NovelVM-snapshot/COPYING.MIT ./NovelVM-snapshot/License\ \(MIT\)
	mv ./NovelVM-snapshot/COPYING.TINYGL ./NovelVM-snapshot/License\ \(TinyGL\)
	$(XCODETOOLSPATH)/SetFile -t ttro -c ttxt ./NovelVM-snapshot/*
	mkdir NovelVM-snapshot/doc
	cp $(srcdir)/doc/QuickStart ./NovelVM-snapshot/doc/QuickStart
	$(XCODETOOLSPATH)/SetFile -t ttro -c ttxt ./NovelVM-snapshot/doc/QuickStart
# NovelVM: missing CpMac in some cases
	cp -R $(bundle_name) ./NovelVM-snapshot/
# NovelVM missing background file:
#	cp $(srcdir)/dists/macosx/DS_Store ./NovelVM-snapshot/.DS_Store
#	cp $(srcdir)/dists/macosx/background.jpg ./NovelVM-snapshot/background.jpg
#	$(XCODETOOLSPATH)/SetFile -a V ./NovelVM-snapshot/.DS_Store
#	$(XCODETOOLSPATH)/SetFile -a V ./NovelVM-snapshot/background.jpg
	hdiutil create -ov -format UDZO -imagekey zlib-level=9 -fs HFS+ \
					-srcfolder NovelVM-snapshot \
					-volname "NovelVM" \
					NovelVM-snapshot.dmg
	rm -rf NovelVM-snapshot

publish-appcast:
	cp dists/macosx/novelvm_appcast.xml www.novelvm.org:/var/www/appcasts/macosx/release.xml


#
# Special target to generate project files for various IDEs
# Mainly Win32-specific
#

# The release branch is in form 'heads/branch-1-4-1', for this case
# $CUR_BRANCH will be equal to '1', for the rest cases it will be empty
CUR_BRANCH := $(shell cd $(srcdir); git describe --all |cut -d '-' -f 4-)

ideprojects: devtools/create_project
ifeq ($(VER_DIRTY), -dirty)
	$(error You have uncommitted changes)
endif
ifeq "$(CUR_BRANCH)" "heads/master"
	$(error You cannot do it on master)
else ifeq "$(CUR_BRANCH)" ""
	$(error You must be on a release branch)
endif
	@echo Creating Code::Blocks project files...
	@cd $(srcdir)/dists/codeblocks && ../../devtools/create_project/create_project ../.. --codeblocks >/dev/null && git add -f engines/plugins_table.h *.workspace *.cbp
	@echo Creating MSVC project files...
	@cd $(srcdir)/dists/msvc && ../../devtools/create_project/create_project ../.. --msvc-version 12 --msvc >/dev/null && git add -f engines/plugins_table.h *.sln *.vcxproj *.vcxproj.filters *.props
	@echo
	@echo All is done.
	@echo Now run
	@echo "\tgit commit -m 'DISTS: Generated Code::Blocks and MSVC project files'"

# Target to create Raspberry Pi zip containig binary and specific README
raspberrypi_dist:
	mkdir -p $(srcdir)/residulvm-rpi
	cp $(srcdir)/backends/platform/sdl/raspberrypi/README.RASPBERRYPI $(srcdir)/novelvm-rpi/README
	cp $(srcdir)/novelvm $(srcdir)/novelvm-rpi
	zip -r novelvm-rpi.zip novelvm-rpi
	rm -f -R novelvm-rpi

# Mark special targets as phony
.PHONY: deb bundle osxsnap install uninstall
