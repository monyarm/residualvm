[Setup]
AppCopyright=2020
AppName=NovelVM
AppVerName=NovelVM
AppPublisher=The NovelVM Team
AppPublisherURL=https://www.novelvm.org/
AppSupportURL=https://www.novelvm.org/
AppUpdatesURL=https://www.novelvm.org/
DefaultDirName={pf}\NovelVM
DefaultGroupName=NovelVM
AllowNoIcons=true
AlwaysUsePersonalGroup=false
EnableDirDoesntExistWarning=false
Compression=lzma
OutputDir=C:\NovelVM
OutputBaseFilename=novelvm-win32
DisableStartupPrompt=true
AppendDefaultDirName=false
SolidCompression=true
DirExistsWarning=no
;SetupIconFile=graphics\novelvm-install.ico
;WizardImageFile=graphics\left.bmp
UninstallDisplayIcon={app}\novelvm.exe

[Icons]
Name: {group}\{cm:UninstallProgram, NovelVM}; Filename: {uninstallexe}
Name: {group}\NovelVM; Filename: {app}\novelvm.exe; WorkingDir: {app}; Comment: novelvm; Flags: createonlyiffileexists; IconIndex: 0
Name: {group}\NovelVM (noconsole); Filename: {app}\novelvm.exe; Parameters: "--no-console"; WorkingDir: {app}; Comment: novelvm; Flags: createonlyiffileexists; IconIndex: 0
Name: {group}\Authors; Filename: {app}\AUTHORS.txt; WorkingDir: {app}; Comment: AUTHORS; Flags: createonlyiffileexists
Name: {group}\Copying; Filename: {app}\COPYING.txt; WorkingDir: {app}; Comment: COPYING; Flags: createonlyiffileexists
Name: {group}\Copying.FREEFONT; Filename: {app}\COPYING.FREEFONT.txt; WorkingDir: {app}; Comment: COPYING.FREEFONT; Flags: createonlyiffileexists
Name: {group}\Copying.OFL; Filename: {app}\COPYING.OFL.txt; WorkingDir: {app}; Comment: COPYING.OFL; Flags: createonlyiffileexists
Name: {group}\Copying.BSD; Filename: {app}\COPYING.BSD.txt; WorkingDir: {app}; Comment: COPYING.BSD; Flags: createonlyiffileexists
Name: {group}\Copying.LGPL; Filename: {app}\COPYING.LGPL.txt; WorkingDir: {app}; Comment: COPYING.LGPL; Flags: createonlyiffileexists
Name: {group}\Copying.ISC; Filename: {app}\COPYING.ISC.txt; WorkingDir: {app}; Comment: COPYING.ISC; Flags: createonlyiffileexists
Name: {group}\Copying.LUA; Filename: {app}\COPYING.LUA.txt; WorkingDir: {app}; Comment: COPYING.LUA; Flags: createonlyiffileexists
Name: {group}\Copying.MIT; Filename: {app}\COPYING.MIT.txt; WorkingDir: {app}; Comment: COPYING.TINYGL; Flags: createonlyiffileexists
Name: {group}\Copying.TINYGL; Filename: {app}\COPYING.TINYGL.txt; WorkingDir: {app}; Comment: COPYING.TINYGL; Flags: createonlyiffileexists
Name: {group}\Copyright; Filename: {app}\COPYRIGHT.txt; WorkingDir: {app}; Comment: COPYRIGHT; Flags: createonlyiffileexists
Name: {group}\KnownBugs; Filename: {app}\KNOWN_BUGS.txt; WorkingDir: {app}; Comment: KNOWN_BUGS; Flags: createonlyiffileexists
;NEWS
Name: {group}\News; Filename: {app}\NEWS.txt; WorkingDir: {app}; Comment: NEWS; Flags: createonlyiffileexists
;QUICKSTART
;README
Name: {group}\Readme; Filename: {app}\README.txt; WorkingDir: {app}; Comment: README; Flags: createonlyiffileexists

[Run]
Filename: {app}\NovelVM.exe; Flags: nowait skipifdoesntexist postinstall skipifsilent
;Creates a symbolic link for standard save games area, under Windows Vista/7
Filename: {cmd}; Parameters: "/c ""mklink /d ""%userprofile%\Saved Games\NovelVM"" ""%appdata%\NovelVM\Saved games"" "" "; MinVersion: 0, 6.1

[UninstallDelete]
Type: files; Name: {app}\ISTool.url

[Dirs]
Name: "{userappdata}\NovelVM"; MinVersion: 0, 1
Name: "{userappdata}\NovelVM\Saved Games"; MinVersion: 0, 1

[Files]
Source: AUTHORS.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.FREEFONT.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.OFL.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.BSD.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.LGPL.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.ISC.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.LUA.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.MIT.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.TINYGL.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYRIGHT.txt; DestDir: {app}; Flags: ignoreversion
Source: KNOWN_BUGS.txt; DestDir: {app}; Flags: ignoreversion
;NEWS
Source: NEWS.txt; DestDir: {app}; Flags: ignoreversion
;QUICKSTART
;README
Source: README.txt; DestDir: {app}; Flags: ignoreversion isreadme
Source: README-SDL.txt; DestDir: {app}; Flags: ignoreversion
Source: novelvm.exe; DestDir: {app}; Flags: ignoreversion
Source: SDL2.dll; DestDir: {app}; Flags: replacesameversion
; Source: SDL2_net.dll; DestDir: {app}; Flags: replacesameversion
; Source: WinSparkle.dll; DestDir: {app}; Flags: replacesameversion

