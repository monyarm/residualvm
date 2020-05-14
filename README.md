NovelVM: Visual Novel interpreter
=================================


  1. [ What is NovelVM?         ](#1-what-is-novelvm)
  2. [ Current state               ](#2-current-state)
  6. [ Configuration               ](#6-configuration)
  7. [ Troubleshooting             ](#7-troubleshooting-known-bugs-issues)
  8. [ Debugging                   ](#8-debugging)
  9. [ Bug reports                 ](#9-bug-reports)
 10. [ Changelog                   ](#10-changelog)
 11. [ Credits                     ](#11-credits)
 12. [ Contact                     ](#12-contact)


## 1. What is NovelVM?
NovelVM is a game engine reimplementation that allows you to play Visual Novel games.

## 2. Current state
Currently not playable.

### 2.1. Which games does NovelVM support? ###

None

## 6. Configuration
Currently, not all the settings for NovelVM are available through the
GUI, if you have problems with getting anything to work, first try to pass
the settings from the command line, then, if that doesn't work, try to
change your configuration file manually.

### 6.1. Location of configuration file ###

By default, the configuration file is saved in and loaded from:

Operating System     | Location
-------------------- | -----------------------------------------------------------------------------
Windows Vista/7/8/10 | `\Users\username\AppData\Roaming\NovelVM\novelvm.ini`
Windows 2000/XP      | `\Documents and Settings\username\Application Data\NovelVM\novelvm.ini`
Linux                | `~/.config/novelvm/novelvm.ini`
macOS/OS X           | `~/Library/Preferences/NovelVM Preferences`
Others               | `novelvm.ini` in the current directory

### 6.2. Location of saved screenshots ###

By default, screenshots will be saved to:

Operating System    | Location
------------------- | -----------------------------------------------------------------
Windows             | `\Users\username\My Pictures\NovelVM Screenshots`
macOS X             | On the Desktop
Other unices        | In the XDG Pictures user directory, e.g. `~/Pictures/NovelVM Screenshots`
Any other OS        | In the current directory

Alternatively, you can specify the directory where the screenshots will be saved in the configuration file. To do so, add a screenshotpath value under the [novelvm] section:

```
[novelvm]
screenshotpath=/path/to/screenshots/
```

### 6.3. Interesting settings for GrimE games ###

The following settings are currently available in the config file, however
some of them might not work with your current build and some of them might
make NovelVM crash, or behave in weird ways.

Setting           | Values                   | Effect
----------------- | ------------------------ | ---------------------------------------------------
`show_fps`        | `true`/`false`           | If set to `true`, NovelVM will show the current FPS rate while you play
`last_set`        | set name                 | The set you were last on, NovelVM will try to continue from there
`last_save`       | save number              | The save you last saved, NovelVM will have that selected the next time you try to load a game
`use_arb_shaders` | `true`/`false`           | If set to `true` and if you are using the OpenGL renderer NovelVM will use ARB shaders. While fast, they may be incompatible with some graphics drivers
`fullscreen_res`  | `desktop`/width`x`height | If set to `desktop` (the default), NovelVM will use your desktop resolution in fullscreen mode. If set to a resolution such as `640x480` or `1280x720`, that resolution will be used


## 7. Troubleshooting, known bugs, issues
Grim Fandango had a few issues when it came out and a few new and
exciting issues when you try to run it on newer hardware. Some of these
have been fixed in NovelVM, but NovelVM itself also has a few new
issues that we know about:
https://github.com/novelvm/novelvm/blob/master/KNOWN_BUGS

Look below for help with these issues and if you can't find help here,
try contacting us at [Contact](#10-contact) section.

### 7.1. I played a bit, but can't start a new game! ###

This is because the last save and visited scene is stored in your
configuration file, either delete your Grim Fandango entry from the
NovelVM menu and readd it, or go to your configuration file and clean
out the `last_save` and `last_set` entries.

### 7.2. My saved games don't work any more! ###

Did you recently update to a newer build of NovelVM?
NovelVM is still a work in progress, which means that the save format
might change between builds. While attempts are made to keep save file
compatibility, this isn't always possible.


## 8. Debugging
**WARNING:** This section contains information about the various tools that
are included for debugging NovelVM, this should not be necessary for
normal play at all! However, the curious might like to know how to access
these tool. Please use at your own risk!

To enter the debug console, press `Ctrl` + `Alt`  + `d`. Use the `help` command to
display a list of the available commands. To exit, press `Escape` or type
`exit` or `quit`.

Debug console commands common to all engines:

Command             | Description
------------------- | -----------------------------------------------------------
`openlog`           | Show the log of errors/warnings/information from the engine
`debuglevel`        | List or change verbosity of debug output
`debugflag_list`    | List the available debug flags and their status
`debugflag_enable`  | Enable a given debug flag
`debugflag_disable` | Disable a given debug flag
`md5`               | Calculates MD5 checksum of a file
`md5mac`            | Calculates MD5 checksum of a file (Mac format)

### 8.1. Debugging GrimE games ###

The development console can be used to debug both Grim Fandango and Escape
from Monkey Island.

Some of the useful debug console commands:

Command        | Description
-------------- | -----------------------------------------------------------
`jump`         | Jump to a section of the game
`lua_do`       | Execute a lua command
`set_renderer` | Select a renderer (software, OpenGL or OpenGL with shaders)

The `jump` targets can be found at:
  * https://wiki.novelvm.org/index.php/Grim_Fandango_Debug_Mode#Jump_targets
  * https://wiki.novelvm.org/index.php/Escape_From_Monkey_Island_Debug_Mode#Jump_targets

### 8.2. Debugging Grim Fandango ###

Grim Fandango also includes a built in debug mode. To enable debug mode
and debug keys, you will have to add the following line to your
configuration file under the Grim Fandango entry:

`game_devel_mode=true`

Development/debug keys from the original game:

Key           | Binding
------------- | ------------------------------------
`Ctrl` + `e`  | Enter lua string to execute
`Ctrl` + `g`  | Jump to set
`Ctrl` + `i`  | Toggle walk boxes
`Ctrl` + `l`  | Toggle lighting
`Ctrl` + `n`  | Display background name
`Ctrl` + `o`  | Create a door
`Ctrl` + `p`  | Execute patch file
`Ctrl` + `s`  | Turn on cursor
`Ctrl` + `u`  | Create a new object
`Ctrl` + `v`  | Print the value of a variable
`Alt` + `n`   | Next viewpoint
`Alt` + `p`   | Prev viewpoint
`Alt` + `s`   | Run lua script
`Shift` + `n` | Next set
`Shift` + `p` | Previous set
`Shift` + `o` | Toggle object names
`F3`          | Toggle sector editor
`Home`        | Go to default position in current set
`j`           | Enter jump number

Note that these are only available after enabling debug mode.

### 8.3. Debugging Myst III ###

The most useful debug console commands are:

Command       | Description
------------- | ------------------------------------------------------------
`dumpArchive` | Extract a game archive
`infos`       | Display the name of a node and show the associated scripts
`go`          | Jump to a node
`var`         | Display or alter the value of a variable from the game state

### 8.4. Debugging The Longest Journey ###

The debug console commands are:

Command               | Description
--------------------- | --------------------------------------------------------
`changeChapter`       | Change the current chapter
`changeKnowledge`     | Change the value of some knowledge
`changeLocation`      | Change the current location
`chapter`             | Display the current chapter
`decompileScript`     | Decompile a script
`dumpArchive`         | Extract a game archive
`dumpGlobal`          | Print the global level's resource sub-tree to stdout
`dumpKnowledge`       | Print the current knowledge to stdout
`dumpLevel`           | Print the current level's resource sub-tree to stdout
`dumpLocation`        | Print the current location's resource sub-tree to stdout
`dumpRoot`            | Print the resource tree root to stdout
`dumpStatic`          | Print the static level's resource sub-tree to stdout
`enableInventoryItem` | Enable a specific inventory item
`enableScript`        | Enable or disable script
`extractAllTextures`  | Extract the textures used by the 3d models to `dump/`
`forceScript`         | Force the execution of a script
`forceAnimation`      | Force an animation to play
`listAnimations`      | List all the animations in the current level
`listInventoryItems`  | List all inventory items in the game
`listLocations`       | List all the locations in the game
`listScripts`         | List all the scripts in current level
`location`            | Display the current location
`testDecompiler`      | Test decompilation of all the scripts in game

### 8.5. Modding The Longest Journey ###

NovelVM can load replacement assets instead of the original files for
some of the asset types. By leveraging this capability, users can create
mods for the game. These are the currently supported modding features:

  * Load mods from the `mods` directory inside the game data path.
  Each mod should be in its own directory in the `mods` subdirectory.
  Mods are loaded in alphabetical order.

  * Load external PNG files instead of the XMG files inside the game
  archives.
    The replacement PNG files can have larger dimensions when compared to
  the original XMG images, enabling the creation of a high resolution mod.
    The game looks for the replacement files in a mod directory and then
  in the `xarc` subdirectory of the directory containing the archive in
  which the XMG picture to be replaced is located. For instance:
  `mods/[my_mod]/1e/00/xarc/fountain_layercenter.png` needs to be used for
  the Venice park background.
    NovelVM expects PNGs to be in pre-multiplied alpha format for improved
  load times. However the `replacement_png_premultiply_alpha` `novelvm.ini`
  setting allows to load regular transparency PNGs when set to `true` for
  convenience when testing.

  * Load replacement video files for the Smacker animations.
    The replacement files can be either in Smacker or Bink encoding. With
  Smacker, only 1-bit transparency can be used. Transparent pixels must have
  the Cyan color (#00FFFF). When using Bink, 8-bit transparency can be used.
  The alpha channel should be encoded in the pre-multiplied alpha format.
    The replacement videos can have larger dimensions than the originals
  but must have the same number of frames and the same frame rate.
    Like with PNG files, replacement video files are loaded from mod folders:
  for instance `mods/[my_mod]/08/02/xarc/011001.bik` is the animation where
  the tree spirit lifts the egg back into the nest.

  * Load replacement textures for the 3d models.
    Each original `tm` file contains several textures, each with its
  associated mipmaps. The replacement files are `zip` archives containing
  `dds` packaged textures. The replacement archives must be placed at the root
  of the mod directory and be named after the `tm` file they replace:
  `mods/[my_mod]/april_waitress.tm.zip`.
    Each `zip` archive must contain all the textures from the replaced `tm`
  file. The textures need to be encoded in uncompressed RGB or RGBA `dds`
  files with mipmaps. Files inside the archive must be named according
  to the replaced texture name, but with the `bmp` extension replaced with
  `dds`: `backdress-highres-battic.dds`
    The `extractAllTextures` console command can be used to extract the `tm`
  files to `png` files.

Contact us if you need further capabilities for your mod.

## 9. Bug reports
NovelVM still has a few bugs, many might already have been reported,
but should you find a new one, don't hesitate to report it.

### 9.1. How and where do I report bugs? ###

You can report bugs at our GitHub issue tracker:
https://github.com/novelvm/novelvm/issues

Please read the Wiki regarding how to report bugs properly first though:
https://wiki.novelvm.org/index.php?title=Reporting_Bugs

Remember to always provide the following information in your bug reports:
  * Information about the game (e.g. *Escape from Monkey Island, PS2
    version*).
  * Language of game (*English*, *German*, *...*).
  * Platform (*Windows*, *Linux*, *macOS*, *...*).
  * Bug details, including instructions on reproducing it.
  * Preferably also a link to a save game right before the bug happened.


## 10. Changelog

Please refer to our extensive Changelog [here](NEWS.md).

## 11. Credits

Please refer to our extensive Credits list [here](AUTHORS).

## 12. Contact
  * Homepage: https://www.novelvm.org/
  * Wiki: https://wiki.novelvm.org/
  * Discord: https://discord.gg/VFQjRfa
  * Forums: https://forums.novelvm.org/
  * IRC: #novelvm on freenode
