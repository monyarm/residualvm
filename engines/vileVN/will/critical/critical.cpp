/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "critical.h"

CriticalPoint::CriticalPoint(uString Path) : EngineWill(640,480) {
	// Add resources
	AddBGM(new ArchiveWillARC(Path+"bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"voice.arc"));
	AddSE(new ArchiveWillARC(Path+"se.arc"));
	AddScripts(new ArchiveWillARC(Path+"rio.arc"));
	AddImages(new ArchiveWillARC(Path+"chip.arc"));
	AddOther(new ArchiveWillARC(Path+"chip.arc"));
	AddVideo(new ArchiveWillARC(Path+"ending.dat"));
	AddVideo(new ArchiveFiles(Path+"peach.dat"));

	// Configure textview
	//DestroyWidget(textview);
	//textview=new YumeTextview(this);
	//AddWidget(textview,VL_TEXTVIEW);

	// Configure textview
	//selection->Move(100,396);
	//selection->Resize(440,70);
	selection->Resize(640/3,480/3);
	selection->Move(640/3,480/3);
	selection->SetAlignment(HA_CENTER,VA_CENTER);

	// Show main menu
	EventGameDialog(VD_TITLE);
}

const uString CriticalPoint::NativeID(){
	return "Critical";
}

const uString CriticalPoint::NativeName(){
	return "Critical Point";
}


