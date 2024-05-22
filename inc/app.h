#ifndef APP_H 
#define APP_H

#include <windows.h>

// This header works with sym.c which defines the Vanilla symbols
// This header will be split up as it becomes larger

// ### Variables ###
extern UINT32 was_passed_from_cmdline;
extern char input_path;
extern BOOL is_run_from_gui;

extern char current_directory;

extern BOOL global_1;
extern BOOL global_2;
extern UINT32 global_3;
extern UINT32 global_4;
extern short some_index;

extern UINT32 scr_files_produced;

// ### Functions ###
int app_start(void);

extern void og_compile_script();

#endif
