# Special target to create an AmigaOS snapshot installation.
# AmigaOS shell doesn't like indented comments.
amigaosdist: $(EXECUTABLE) $(PLUGINS)
	mkdir -p $(AMIGAOSPATH)
	mkdir -p $(AMIGAOSPATH)/themes
	mkdir -p $(AMIGAOSPATH)/extras
	$(STRIP) $(EXECUTABLE) -o $(AMIGAOSPATH)/$(EXECUTABLE)
	cp ${srcdir}/icons/novelvm_drawer.info $(AMIGAOSPATH).info
	cp ${srcdir}/icons/novelvm.info $(AMIGAOSPATH)/$(EXECUTABLE).info
	cp $(DIST_FILES_THEMES) $(AMIGAOSPATH)/themes/
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(AMIGAOSPATH)/extras/
endif
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(AMIGAOSPATH)/extras/
endif
ifdef DIST_FILES_VKEYBD
	cp $(DIST_FILES_VKEYBD) $(AMIGAOSPATH)/extras/
endif
	cat ${srcdir}/README.md | sed -f ${srcdir}/dists/amiga/convertRM.sed > README.conv
# AmigaOS's shell is not happy with indented comments, thus don't do it.
# AREXX seems to have problems when ${srcdir} is '.'. It will break with a
# "Program not found" error. Therefore we copy the script to the cwd and
# remove it again, once it has finished.
	cp ${srcdir}/dists/amiga/RM2AG.rexx .
	rx RM2AG.rexx README.conv
	cp README.guide $(AMIGAOSPATH)
	rm RM2AG.rexx
	rm README.conv
	rm README.guide
	cp $(DIST_FILES_DOCS) $(AMIGAOSPATH)

# Special target to cross create an AmigaOS snapshot installation
amigaoscross: $(EXECUTABLE)
	mkdir -p NovelVM
	mkdir -p NovelVM/themes
	mkdir -p NovelVM/extras
	cp $(EXECUTABLE) NovelVM/NovelVM
	cp ${srcdir}/icons/novelvm_drawer.info NovelVM.info
	cp ${srcdir}/icons/novelvm.info NovelVM/NovelVM.info
	cp $(DIST_FILES_THEMES) NovelVM/themes/
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) NovelVM/extras/
endif
	cp $(srcdir)/AUTHORS NovelVM/AUTHORS.txt
	cp $(srcdir)/COPYING NovelVM/COPYING.txt
	cp $(srcdir)/COPYING.BSD NovelVM/COPYING.BSD.txt
	cp $(srcdir)/COPYING.LGPL NovelVM/COPYING.LGPL.txt
	cp $(srcdir)/COPYING.FREEFONT NovelVM/COPYING.FREEFONT.txt
	cp $(srcdir)/COPYING.ISC NovelVM/COPYING.ISC.txt
	cp $(srcdir)/COPYING.LUA NovelVM/COPYING.LUA.txt
	cp $(srcdir)/COPYING.MIT NovelVM/COPYING.MIT.txt
	cp $(srcdir)/COPYING.TINYGL NovelVM/COPYING.TINYGL.txt
	cp $(srcdir)/COPYRIGHT NovelVM/COPYRIGHT.txt
	cp $(srcdir)/KNOWN_BUGS NovelVM/KNOWN_BUGS.txt
	cp $(srcdir)/NEWS.md NovelVM/NEWS.txt
	cp $(srcdir)/doc/QuickStart NovelVM/QuickStart.txt
	cp $(srcdir)/README.md NovelVM/README.txt
	zip -r novelvm-amigaos4.zip NovelVM NovelVM.info
