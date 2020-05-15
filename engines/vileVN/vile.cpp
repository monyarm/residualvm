
void ViLE::RunEngine(EngineVN *engine){
	// Load screen surface and set a caption
	SDL_Surface *screen=SDL_GetVideoSurface();
	uString caption=engine->NativeName();
	if(caption!=PACKAGE_STRING){
		uString pstring=uString(PACKAGE_STRING);
		uString pcode=uString(PACKAGE_LINE);
		caption=pstring+uString(" (")+pcode+uString("): ")+caption;
	}
	else{
		uString pstring=uString(PACKAGE_STRING);
		uString pcode=uString(PACKAGE_LINE);
		caption=pstring+uString(" (")+pcode+uString(")");
	}
	SDL_WM_SetCaption(caption.c_str(),NULL);

	// Discard old and fake events
	SDL_Event event;
	while(SDL_PollEvent(&event));

	// Load system settings
	engine->EventLoadSystem();

	// Drive engine
	while(engine){
		// Tick engine
		engine->EventHostTick();

		// Redraw screen
		if(engine->EventHostPaint(screen)){
			SDL_Flip(screen);
		}

		// Handle events
		while(engine && SDL_PollEvent(&event)){
			if(event.type==SDL_MOUSEMOTION){
				// Basic mousemove event
				engine->EventHostMouseMove(
						screen,event.motion.x,
						event.motion.y);
			}
			else if(event.type==SDL_MOUSEBUTTONDOWN){
				// Left mouse button
				if(event.button.button==SDL_BUTTON_RIGHT){
					engine->EventHostMouseRightDown(
							screen,event.motion.x,
							event.motion.y);
				}
				else{
					engine->EventHostMouseLeftDown(
							screen,event.motion.x,
							event.motion.y);
				}
			}
			else if(event.type==SDL_MOUSEBUTTONUP){
				// Left mouse button
				if(event.button.button==SDL_BUTTON_RIGHT){
					engine->EventHostMouseRightUp(
						screen,event.motion.x,
						event.motion.y);
				}
				else{
					engine->EventHostMouseLeftUp(
							screen,event.motion.x,
							event.motion.y);
				}
			}
			else if(event.type==SDL_KEYDOWN){
				engine->EventHostKeyDown(event.key.keysym.sym);
			}
			else if(event.type==SDL_KEYUP){
				engine->EventHostKeyUp(event.key.keysym.sym);
			}
			else if(event.type==SDL_QUIT){
				if(engine->GetShutdown()){
					// Shut down engine and break out of loop
					engine->EventSaveSystem();
					delete engine;
					engine=0;
				}
				else{
					// Request shutdown
					engine->EventGameDialog(VD_SHUTDOWN);
				}
			}
		}

		// Purge deleted objects
		Group::Purge();
	}
}

EngineVN *ViLE::LoadEngine(uString Path){
	// Assert trailing separator
	Path=EDL_StripPath(Path);
	int i=Path.length();
	if(i && Path[i-1]!='/' && Path[i-1]!='\\'){
#ifdef VILE_ARCH_MICROSOFT
		Path+="\\";
#else
		Path+="/";
#endif
	}

	// Try to load a game
	EngineVN *engine=0;
	if(0){
	}
#ifdef VILE_SUPPORT_WINDY
	else if(ProbeNocturnal(Path))				engine=new Nocturnal(Path);
	else if(ProbeMayclub(Path))					engine=new Mayclub(Path);
#endif
#ifdef VILE_SUPPORT_JAST
	else if(ProbeJUMC(Path))					engine=new JUMC(Path);
#endif
#ifdef VILE_SUPPORT_CWARE
	else if(ProbeDiviDead(Path))				engine=new DiviDead(Path);
#endif
#ifdef VILE_SUPPORT_WILL
	else if(ProbeYumeMiruKusuri(Path))			engine=new YumeMiruKusuri(Path);
	else if(ProbeCriticalPoint(Path))			engine=new CriticalPoint(Path);
#endif
#ifdef VILE_SUPPORT_TLOVE
	else if(ProbeTrueLove(Path))				engine=new Truelove(Path);
#endif
#ifdef VILE_SUPPORT_IKURA
	else if(ProbeCrescendo(Path))				engine=new Crescendo(Path);
	else if(ProbeSagara(Path))					engine=new Sagara(Path);
	else if(ProbeSnow(Path))					engine=new Snow(Path);
	else if(ProbeKanaOkaeri(Path))				engine=new KanaOkaeri(Path);
	else if(ProbeKana(Path))					engine=new Kana(Path);
	else if(ProbeHitomi(Path))					engine=new Hitomi(Path);
	else if(ProbeCatGirl(Path))					engine=new CatGirl(Path);
	else if(ProbeIdols(Path))					engine=new Idols(Path);
#endif
#ifdef VILE_SUPPORT_CROWD
	else if(ProbeTokimeki(Path))				engine=new Tokimeki(Path);
	else if(ProbeXChange1(Path))				engine=new XChange1(Path);
	else if(ProbeXChange3(Path))				engine=new XChange3(Path);
#endif
	else										;
	return engine;
}

/*! \brief Initializes (or resizes) video the display surface
 *  \return True if the video surface could be created
 *
 *  This method will use the configuration settings, and can be executed
 *  multiple times in case you want to reload the settings and resize the 
 *  display.
 */
bool ViLE::InitVideo(){
	// Assert configuration
	bool retval=false;
	static Uint32 ow=0;
	static Uint32 oh=0;
	Uint32 w=Cfg::Display::Width;
	Uint32 h=Cfg::Display::Height;
	Uint32 d=Cfg::Display::Depth;
	Uint32 f=Cfg::Display::Flags;
	if(!w || !h){
		w=DEFAULT_WIDTH;
		h=DEFAULT_HEIGHT;
	}

	if(w && h && ow==w && oh==h){
		// Changing size to the same size ...
		retval=true;
	}
	else if(SDL_SetVideoMode(w,h,d,f)!=0){
		// Resized surface
		retval=true;
		ow=w;
		oh=h;
	}
	else{
		LogError("Failed to create display surface: %s",SDL_GetError());
	}
	return retval;
}

bool ViLE::InitSystem(Uint32 Systemflags){
	// Initialize SDL and load application object
	bool retval=false;
	if (SDL_Init(Systemflags)<0) {
		LogError("SDL Initialization error: %s",SDL_GetError());
	}
	else if(TTF_Init()<0){
		LogError("Could not init TrueType: %s\n",TTF_GetError());
	}
	else if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)==0){
		LogError("Failed to link to any image libraries: %s\n",IMG_GetError());
	}
	else{
#ifdef VILE_FEATURE_FFMPEG
        avcodec_register_all();
		av_register_all();
#endif
		retval=true;
	}
#ifdef VILE_BUILD_W32CONSOLE
	RedirectIOToConsole();
#endif	
	return retval;
}

void ViLE::Quit(){
	// Close internal systems
	Media::Close();

	// Close down dependencies
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void ViLE::Help(){
	// Show available options
	About();
	LogMessage("Usage: vilevn [OPTIONS|COMMAND]");
	LogMessage("");
	LogMessage("Display settings:");
	LogMessage("\t--fullscreen\t\t\tForces fullscreen mode");
	LogMessage("\t--size WIDTH HEIGHT\t\tSet screen size");
	LogMessage("\t--depth 8|16|24|32\t\tSet bit per pixel for screen");
	LogMessage("\t--fontface PATH\t\t\tDefault truetype font file");
	LogMessage("\t--fontsize SIZE\t\t\tDefault font size (pixel height)");
	LogMessage("");
	LogMessage("Audio configuration:");
	LogMessage("\t--disable-audio\t\t\tDo not load any audio resources");
	LogMessage("\t--disable-midi\t\t\tDo not load MIDI tracks or soundfonts");
	LogMessage("\t--cdrom INDEX\t\t\tSet source for CDDA audio (=auto)");
	LogMessage("\t--samplerate SAMPLERATE\t\tSamples per second (=44100)");
	LogMessage("\t--buffersize BYTES\t\tBytes per frame (=1024)");
	LogMessage("\t--buffercount NUMBER\t\tBuffered frames (=4)");
	LogMessage("\t--soundfont FILENAME\t\tSoundfont for MIDI synthesizer");
	LogMessage("");
	LogMessage("Video configuration:");
	LogMessage("\t--disable-video\t\t\tDo not load any video resources");
	LogMessage("\t--disable-overlay\t\tDisable hardware video rendering");
	LogMessage("");
	LogMessage("Other:");
	LogMessage("\t--config PATH\t\t\tSpecify configuration file");
	LogMessage("\t--cwd FOLDER\t\t\tSet current working directory");
	LogMessage("\t--game FOLDER\t\t\tLoad game from specified path");
	LogMessage("\t--save FOLDER\t\t\tPath for savegames");
	LogMessage("\t--disable-menu\t\t\tDo not show application menu bar");
	LogMessage("\t--logcolor\t\t\tFormat log output with colors");
	LogMessage("\t--logfile PATH\t\t\tSpecify output log file");
	LogMessage("\t--keyfile PATH\t\t\tLoad encryption key from file");
	LogMessage("\t--verbose\t\t\tPrint additional runtime information");
	LogMessage("\t--dryrun\t\t\tLoad as usual but prevent from starting");
	LogMessage("\t--info [title|res|runtime]\tDisplay runtime information");
	LogMessage("");
	LogMessage("Commands:");
#if VILE_FEATURE_UNITTEST
	LogMessage("\t--unittest all||...\t\tUnit-test core class' functionality");
	LogMessage("\t--benchmark all|...\t\tBenchmark core class' performance");
#endif
	LogMessage("\t--list ARC\t\t\tLists resources in archive");
	LogMessage("\t--extract ARC [RES [RES ..]]\tExtract resource from archive");
	LogMessage("\t--decode INFILE [INFILE ..]\tDecode resources");
	LogMessage("\t--xdec ARC [RES [RES ..]]\tExtract AND decode resources");
	LogMessage("\t--archive ARC INFILE [INFILE..]\tArchives resources");
	LogMessage("\t--help\t\t\t\tThis helpfull message");
	LogMessage("\t--about\t\t\t\tAbout ViLE");
	LogMessage("");
	LogMessage("");
	LogMessage("You can also use the following hotkeys at runtime:");
	LogMessage("\tALT+1\t\t\t\tNo graphic filters");
	LogMessage("\tALT+3\t\t\t\tHQ2X graphic filter");
	LogMessage("\tALT+4\t\t\t\tHQ3X graphic filter");
	LogMessage("\tALT+5\t\t\t\tHQ4X graphic filter");
	LogMessage("\tALT+ENTER\t\t\tToggle fullscreen mode");
	LogMessage("\tALT+F4\t\t\t\tExit");
	LogMessage("\tF5\t\t\t\tLoad game");
	LogMessage("\tF6\t\t\t\tSave game");
	LogMessage("\tF7\t\t\t\tOptions dialog");
	LogMessage("\tF8\t\t\t\tTitle screen");
	LogMessage("\tF9\t\t\t\tExit");
	LogMessage("\tF10\t\t\t\tDump screenshot to file");
	LogMessage("\tF11\t\t\t\tToggle debug output");
	LogMessage("");
}

void ViLE::About(){
	LogMessage("");
	LogMessage("Visual Library Engine");
	LogMessage("%s (%s, Rev:%s)",PACKAGE_STRING,PACKAGE_LINE,SCM_VERSION);
	LogMessage("2011 (C) ViLE Team <http://vilevn.org>");
	LogMessage("Released under GNU GPLv3 <http://gnu.org/licenses/gpl.html>");
	LogMessage("");
}

void ViLE::InfoTitle(EngineVN *Engine){
	LogMessage("Game information:");
	LogMessage("\tID:%s",Engine->NativeID().c_str());
	LogMessage("\tName:%s",Engine->NativeName().c_str());
	LogVerbose("\tDirectory: %s",Cfg::Path::game.c_str());
	LogMessage("\tResolution:%dx%d",
			Engine->NativeWidth(),
			Engine->NativeHeight());
}

void ViLE::InfoResources(EngineVN *Engine){
	Stringlist list;
	LogMessage("Resource information:");
	if(Engine->GetVoices(&list)){
		LogMessage("\tVoices:%s",list.Enumerate().c_str());
	}
	if(Engine->GetScripts(&list)){
		LogMessage("\tScripts:%s",list.Enumerate().c_str());
	}
	if(Engine->GetImages(&list)){
		LogMessage("\tImages:%s",list.Enumerate().c_str());
	}
	if(Engine->GetBGM(&list)){
		LogMessage("\tBGM:%s",list.Enumerate().c_str());
	}
	if(Engine->GetSE(&list)){
		LogMessage("\tSE:%s",list.Enumerate().c_str());
	}
	if(Engine->GetVideo(&list)){
		LogMessage("\tVideo:%s",list.Enumerate().c_str());
	}
	if(Engine->GetOther(&list)){
		LogMessage("\tOther:%s",list.Enumerate().c_str());
	}
}

void ViLE::InfoRuntime(){
	// Dump compiletime information
	LogMessage("Version information");
	LogMessage("\tCompiled: %s",COMPILATION_DATE);
	LogMessage("\tVersion: %s (%s)",PACKAGE_STRING,PACKAGE_LINE);
	LogMessage("\tRevision: %s",SCM_VERSION);
#if VILE_ARCH_MICROSOFT
	LogMessage("\tArch: Windows");
#elif VILE_ARCH_LINUX
	LogMessage("\tArch: Linux");
#elif VILE_ARCH_FREEBSD
	LogMessage("\tArch: FreeBSD");
#else
	LogMessage("\tArch: UNSUPPORTED");
#endif

	// List enabled engines
	LogMessage("\r\nEnabled sub-engines");
#if VILE_SUPPORT_WINDY
	LogMessage("\tWindy");
#endif
#if VILE_SUPPORT_IKURA
	LogMessage("\tIkuraGDL");
#endif
#if VILE_SUPPORT_JAST
	LogMessage("\tJAST Engine");
#endif
#if VILE_SUPPORT_CROWD
	LogMessage("\tCrowd Engine 3");
#endif

	// List linked library version numbers
	LogMessage("\r\nLinked libraries");
	LogMessage("\tSDL:\t\t%d.%d.%d",
			SDL_MAJOR_VERSION,
			SDL_MINOR_VERSION,
			SDL_PATCHLEVEL);
	LogMessage("\tSDL_gfx:\t%d.%d.%d",
			SDL_GFXPRIMITIVES_MAJOR,
			SDL_GFXPRIMITIVES_MINOR,
			SDL_GFXPRIMITIVES_MICRO);
	LogMessage("\tSDL_image:\t%d.%d.%d",
			SDL_IMAGE_MAJOR_VERSION,
			SDL_IMAGE_MINOR_VERSION,
			SDL_IMAGE_PATCHLEVEL);
	LogMessage("\tSDL_ttf:\t%d.%d.%d",
			SDL_TTF_MAJOR_VERSION,
			SDL_TTF_MINOR_VERSION,
			SDL_TTF_PATCHLEVEL);
#if VILE_FEATURE_FLUIDSYNTH
	LogMessage("\tFluidsynth:\t%d.%d.%d",
			FLUIDSYNTH_VERSION_MAJOR,
			FLUIDSYNTH_VERSION_MINOR,
			FLUIDSYNTH_VERSION_MICRO);
#endif
#if VILE_FEATURE_FFMPEG
	LogMessage("\tavcodec:\t%d.%d.%d",
			LIBAVCODEC_VERSION_MAJOR,
			LIBAVCODEC_VERSION_MINOR,
			LIBAVCODEC_VERSION_MICRO);
	LogMessage("\tavformat:\t%d.%d.%d",
			LIBAVFORMAT_VERSION_MAJOR,
			LIBAVFORMAT_VERSION_MINOR,
			LIBAVFORMAT_VERSION_MICRO);
	LogMessage("\tswscale:\t%d.%d.%d",
			LIBSWSCALE_VERSION_MAJOR,
			LIBSWSCALE_VERSION_MINOR,
			LIBSWSCALE_VERSION_MICRO);
#endif
	LogMessage("");
}

bool ViLE::Archive(uString Archive,Stringlist *List){
	Resources resman;
	int count=List->GetCount();
	for(int i=0;i<count;i++){
		resman.AddResource(List->GetString(i));
	}
	return resman.Write(Archive);
}

bool ViLE::Decode(uString InFile){
	bool retval=false;
	Resources resman;
	resman.AddResource(InFile);
	SDL_Surface *image=resman.GetImage(InFile);
	SDL_Surface **animation=resman.GetAnimation(InFile);
	uString OutFile=EDL_FileDirectory(InFile)+EDL_FileName(InFile)+".bmp";
	if(animation){
		char buffer[OutFile.length()+10];
		LogMessage("Converting animation resource to bitmaps: %s -> %s",
				InFile.c_str(),OutFile.c_str());
		for(int i=0;animation[i];i++){
			sprintf(buffer,"%s_%02d.bmp",OutFile.c_str(),i+1);
			SDL_SaveBMP(animation[i],buffer);
			SDL_FreeSurface(animation[i]);
			retval=true;
		}
		delete [] animation;
	}
	else if(image){
		LogMessage("Converting image resource to bitmap: %s -> %s",
				InFile.c_str(),OutFile.c_str());
		SDL_SaveBMP(image,OutFile.c_str());
		SDL_FreeSurface(image);
		retval=true;
	}
	else{
		LogMessage("Not a decodable resource: %s",InFile.c_str());
	}
	return retval;
}

/*! \brief Lists resources in a game archive
 *  \param Archive Archive to list
 *  \return True if any resources where found
 */
bool ViLE::List(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			LogMessage("%s",list.GetString(i).c_str());
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Lists resources in a game archive
 *  \param Archive Archive to list
 *  \return True if any resources where found
 */
bool ViLE::Extract(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			uString name=list.GetString(i);
			RWops *res=resman.GetResource(name);
			if(res){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts a resource from a game archive
 *  \param Archive Archive to extract from
 *  \param List Resources to extract
 *  \return True if output file was successfully written
 */
bool ViLE::Extract(uString Archive,Stringlist *List){
	bool retval=false;
	Resources resman;
	if(List && resman.AddResource(Archive)>0){
		int c=List->GetCount();
		for(int i=0;i<c;i++){
			uString name=List->GetString(i);
			RWops *res=resman.GetResource(name);
			if(res){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
			else{
				LogMessage("Invalid resource: %s",name.c_str());
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts AND decodes resources from an archive
 *  \param Archive Archive to extract from
 *  \return True if any resources where found
 */
bool ViLE::XDec(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			char buffer[32];
			uString name=list.GetString(i);
			SDL_Surface **animation;
			SDL_Surface *image;
			RWops *res;
			if((animation=resman.GetAnimation(name))){
				LogMessage("Converting animation to frames: %s",name.c_str());
				for(int i=0;animation[i];i++){
					if(i==0 && animation[1]==0){
						sprintf(buffer,"%s.bmp",name.c_str());
					}
					else{
						sprintf(buffer,"%s_%02d.bmp",name.c_str(),i+1);
					}
					SDL_SaveBMP(animation[i],buffer);
					SDL_FreeSurface(animation[i]);
					retval=true;
				}
				delete [] animation;
			}
			else if((image=resman.GetImage(name))){
				LogTest("Converting %s to bitmap",name.c_str());
				name+=".bmp";
				SDL_SaveBMP(image,name.c_str());
				SDL_FreeSurface(image);
				retval=true;
			}
			else if((res=resman.GetResource(name))){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					delete res;
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts AND decodes resources from an archive
 *  \param Archive Archive to extract from
 *  \param List Resources to extract
 *  \return True if any resources where found
 */
bool ViLE::XDec(uString Archive,Stringlist *List){
	bool retval=false;
	Resources resman;
	if(List && resman.AddResource(Archive)>0){
		int c=List->GetCount();
		for(int i=0;i<c;i++){
			char buffer[32];
			uString name=List->GetString(i);
			SDL_Surface **animation;
			SDL_Surface *image;
			RWops *res;
			if((animation=resman.GetAnimation(name))){
				LogMessage("Converting animation to frames: %s",name.c_str());
				for(int i=0;animation[i];i++){
					if(i==0 && animation[1]==0){
						sprintf(buffer,"%s.bmp",name.c_str());
					}
					else{
						sprintf(buffer,"%s_%02d.bmp",name.c_str(),i+1);
					}
					SDL_SaveBMP(animation[i],buffer);
					SDL_FreeSurface(animation[i]);
					retval=true;
				}
				delete [] animation;
			}
			else if((image=resman.GetImage(name))){
				LogTest("Converting %s to bitmap",name.c_str());
				name+=".bmp";
				SDL_SaveBMP(image,name.c_str());
				SDL_FreeSurface(image);
				retval=true;
			}
			else if((res=resman.GetResource(name))){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					delete res;
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}
