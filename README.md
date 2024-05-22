miss2-patches
==================

Patches to 'miss2', the GTA2 Script Compiler.

`miss2.exe` by default is a graphical application despite the fact all it does
is take an input file and produce an output file while outputting text.

This set of patches removes the GUI part and replaces it with a CLI.

Patches
-------
- [X] automatically compile file passed from command line on startup and then exit
- [X] redirect compilation output to stdout/stderr instead of listbox
- [X] return 0 or 1 retcode depending if compilation suceeds or not
- [X] disable gui completely
- [X] output '.scr' instead of '.SCR'
- [X] output '.tmp' instead of '.TMP'
- [X] output '.txt' instead of '.TXT'
- [X] fix current working dir nonsense `GetCurrentDirectoryA` by changing to dir where app is in
- [X] copy target file to appdir, then copy back again
- [ ] make output truly deterministic

Authors
-------
 - Vorschreibung

Building on *nix
-----------------
 - Install [petool](https://github.com/FunkyFr3sh/petool) from github
 - Get mingw-w64 from your package manager
 - Get NASM
 - make or gmake

Building on Windows
-------------------
 - Download [w64devkit-mini-for-patching.zip](https://github.com/FunkyFr3sh/petool/releases/latest/download/w64devkit-mini-for-patching.zip) ([.7z](https://github.com/FunkyFr3sh/petool/releases/latest/download/w64devkit-mini-for-patching.7z))
 - Extract `w64devkit-mini-for-patching.zip` to `C:\`
 - run `build.cmd`
