# fiflo - changelog

## Description
Below version numbers contains more detailed descriptions of annotated Git
tags. Dates are not alternative version names. They are just release dates.
Changelog contains only new tangible features. To see what's new in source
files use git-show or git-log.

## Versions:
### v2.0.0 (2018, WIP):
- vertical && horizontal scroll,
- lines as separate strings,
- no 80 chars per line limit,
- arrows are ignored,
- line numbers,
- better performance (2018, block-allocated memory),
- fixed buffer of size 32M.

### v1.1.0 (2018, August 12):
- dynamically allocated text (2018, in theory max. 2G - 1),
- real stdin (2018, built-in terminal) cursor,
- version info,
- soft horizontal scrolling,
- file is no longer created at the program's start,
- folder as the file prevention,
- auto_newline when the single line is equal <user's terminal width>.

### v1.0.0 (2018, July 15):
- simplified API,
- less memory usage,
- proper text allocation,
- faster initialization,
- usable for the first time,
- a lot of bugs removed,
- UI redesigned,
- 80 chars per line limit && auto newline,

### v0.5.0-unstable (2018, July 1):
(2018, The last v0.y.z version and showup of the old program logic. Mostly insecure.
Not merged with "master" so look for it with tags.)
- multiple lines writing && rendering,
- file input/output operations,
- man-page added,
- upper bar is back,
- absolute paths support,
- OS check.

### v0.4.3 (2018, June 10):
- fnfo about min term size fix.

### v0.4.2 (2018, June 10):
- makefile update.

### v0.4.1 (2018, June 10):
- docs update.

### v0.4.0 (2018, June 10):
- rendering a single line of typed chars/strings,
- UI redesigned,
- cursor added,
- makefile && similar ones improved.

### v0.3.0 (2018, May 31):
- printing only pressed char on the screen,
- working char counter with under/overflow protection,
- rendering in a one frame,
- full terminal size checking.

### v0.2.0 (2018, May 27):
- version deleted from the upper bar,
- program's name centering in the upper bar fixed,
- basic *nixGetch implemented.

### v0.1.1 (2018, May 27):
- branches are now merged, not rebased.

### v0.1.0 (2018, May 27):
- program's name and version in upper bar,
- changelog added,
- windows size checking,
- changelog improvements.

### v0.0.0 (2018, May 27):
- initial release (basic window rendering).