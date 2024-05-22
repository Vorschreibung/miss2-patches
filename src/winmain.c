#include "macros/patch.h"
#include "app.h"

#include <windows.h>
#include <shlwapi.h>

#include "./globals.h"
#include "./compile.c"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
   // "cli"
   if (cmdline[0] == '\0') {
      fprintf(stderr, "usage: <mis-file-path>\n");
      exit(1);
   }

   if (!FileExists(cmdline)) {
      printf("ERROR: Input file does not exist: '%s'\n", cmdline);
      exit(1);
   }

   global_cmdline = cmdline;
   strcpy(&input_path, global_cmdline);

   global_debug = FALSE;

   // change directory to where the miss2.exe sits, the program crashes
   // otherwise
   { 
      char module_name [MAX_PATH];
      if (!GetModuleFileNameA(hInst, module_name, MAX_PATH)) 
      {
         printf("ERROR: Failed to get module_name\n");
         exit(1);
      }

      char module_dir [MAX_PATH];
      strcpy(module_dir, module_name);
      if (!PathRemoveFileSpecA(module_dir)) 
      {
         printf("ERROR: Failed to get module_dir\n");
         exit(1);
      }

      // set directory
      if (!SetCurrentDirectory(module_dir))
      {
         printf("ERROR: SetCurrentDirectory failed (%ld)\n", GetLastError());
         exit(1);
      }
   }

   return app_start();
}
