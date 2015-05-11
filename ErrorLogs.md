# Introduction #

This section is intended for advanced users, or users that are trying to contribute to development via the [issue tracker](http://code.google.com/p/prssr/issues/list).

# Details #

To start logging go to the registry HKCU\Software\DaProfik\pRSSreader.  Set LogFile (STRING) to the file name with log (e.g. \prssr.log will log to that file, you can point this file to the sd card as well).  Set LogLevel (DWORD) to something other than 0.
  * 0 - no logging
  * 1 - basic logging
  * 3 - more logging
  * 5 - debug logging
  * 7 - psychotic logging (be careful you have plenty of disk space available)

Usually 5 is good enough. Since you are logging to a file, pRSSreader
will be a little bit lazier.

Good idea is to open a bug report in the issue tracker and attach the
log file along with it.