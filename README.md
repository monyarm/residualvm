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
