#include "macros/patch.h"
#include "app.h"

#include <windows.h>
#include <shlwapi.h>
#include "stdio.h"

#include "./utils.c"
#include "./globals.h"

// reference documentation: https://github.com/FunkyFr3sh/petool

// miss2.exe by default is a graphical application despite the fact all it does
// is take an input file and produce an output file while outputting text
// 
// this set of patches removes the GUI part and replaces it with a CLI
//
// @TODO [X] automatically compile file passed from command line on startup and then exit
// @TODO [X] redirect compilation output to stdout/stderr instead of listbox
// @TODO [X] return 0 or 1 retcode depending if compilation suceeds or not
// @TODO [X] disable gui completely
// @TODO [X] output '.scr' instead of '.SCR'
// @TODO [X] output '.tmp' instead of '.TMP'
// @TODO [X] output '.txt' instead of '.TXT'
// @TODO [X] fix current working dir nonsense `GetCurrentDirectoryA` by changing to dir where app is in
// @TODO [X] copy target file to appdir, then copy back again
// @TODO [ ] make output truly deterministic

// change file output extension from '.TMP' to '.tmp'
SETBYTE(0x0041fa28 + 3, 0x74); // t
SETBYTE(0x0041fa32 + 3, 0x6d); // m
SETBYTE(0x0041fa3c + 3, 0x70); // p

// // change file output extension from '.TXT' to '.txt'
SETBYTE(0x00403cb3 + 7, 0x74); // t
SETBYTE(0x00403cc1 + 7, 0x78); // x
SETBYTE(0x00403ccf + 7, 0x74); // t
//
// change file output extension from '.SCR' to '.scr'
SETBYTE(0x004210e8 + 3, 0x73); // s
SETBYTE(0x004210f2 + 3, 0x63); // c
SETBYTE(0x004210fc + 3, 0x72); // r


static int currently_erroring = 0;
static int has_errored = 0;

// print to stdout instead to Window
CALL_NOP(0x00421014, _log_print);
EXTERN_C LRESULT WINAPI log_print(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   // overrides a SendMessageA call which adds an entry to the listbox output
   // and instead prints to stdout/stderr
   if (Msg == 384) {
      if (currently_erroring) {
         fprintf(stderr, "%s\n", (char*)(lParam));
      } else {
         fprintf(stdout, "%s\n", (char*)(lParam));
      }
   }

   return 0;
}

// on compilation individual error hook hook
extern void og_on_error(int param_1, char *param_2, UINT32 param_3, UINT32 param_4, UINT32 param_5);
CALL(0x00410cdf, _wrapped_on_error);
CALL(0x00410d11, _wrapped_on_error);
CALL(0x00410d53, _wrapped_on_error);
CALL(0x00410da2, _wrapped_on_error);
CALL(0x00410db8, _wrapped_on_error);
EXTERN_C void wrapped_on_error(int param_1, char *param_2, UINT32 param_3, UINT32 param_4, UINT32 param_5) {
   currently_erroring = 1;
   og_on_error(param_1, param_2, param_3, param_4, param_5);
   currently_erroring = 0;
   has_errored = 1;
}

// disable built-in command line processing of MISS2
extern char ** acmdln;
CALL_NOP(0x0047bbcd, _get_main_args);
EXTERN_C int get_main_args(int * argc, char *** argv, char *** env, int doWildCard, void* startInfo) {
   *acmdln = "";
   return 0;
}

CALL(0x0047bc47, _main_window);
EXTERN_C void main_window() {
   // init some globals
   {
      GetCurrentDirectoryA(0x100, &current_directory);

      global_3 = 0xfffffff;
      global_4 = 1;

      was_passed_from_cmdline = 0;
      global_1 = 0;
      global_2 = 0;
      scr_files_produced = 0;
      some_index = 0;
      is_run_from_gui = TRUE;
   }


   char* input_mispath = global_cmdline;

   // compile new file name paths
   char output_scrpath[MAX_PATH];
   strcpy(output_scrpath, input_mispath);
   PathRenameExtensionA(output_scrpath, ".scr");

   char tmp_misname[MAX_PATH];
   { // find random tmp filename
      do {
         *(tmp_misname + 0) = 't';
         *(tmp_misname + 1) = 'm';
         *(tmp_misname + 2) = 'p';
         *(tmp_misname + 3) = '-';
         rand_str(tmp_misname+4, 10);
         *(tmp_misname + 14) = '.';
         *(tmp_misname + 15) = 'm';
         *(tmp_misname + 16) = 'i';
         *(tmp_misname + 17) = 's';
         *(tmp_misname + 18) = '\0';
      } while (FileExists(tmp_misname));
   }


   char tmp_mispath[MAX_PATH];
   PathCombineA(
         tmp_mispath,
         &current_directory,
         tmp_misname
   );

   // trying to avoid race conditions
   touch_file(tmp_mispath);

   char tmp_scrpath[MAX_PATH];
   strcpy(tmp_scrpath, tmp_mispath);
   PathRenameExtensionA(tmp_scrpath, ".scr");

   // copy target file to tmpfile
   copy_file(global_cmdline, tmp_mispath);
   printf("Original .mis: %s\n", global_cmdline);
   strcpy(&input_path, tmp_mispath);


   // compile
   og_compile_script();

   if (!global_debug) {
      cleanup_file(tmp_mispath);
   }

   if (has_errored) {
      exit(1);
   } else {
      // copy tmp_scrpath to output_scrpath
      copy_file(tmp_scrpath, output_scrpath);
      printf("Copied '%s' to target '%s'\n", tmp_scrpath, output_scrpath);

      if (!global_debug) {
         cleanup_file(tmp_scrpath);
      }

      exit(0);
   }
}
