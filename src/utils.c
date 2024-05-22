#ifndef UTILS_C
#define UTILS_C

#include <stdio.h>
#include <windows.h>

BOOL FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (
     dwAttrib != INVALID_FILE_ATTRIBUTES && !(
        dwAttrib & FILE_ATTRIBUTE_DIRECTORY
     )
  );
}

void rand_str(char *dest, size_t length)
{
   char charset[] = "0123456789"
      "abcdefghijklmnopqrstuvwxyz";

   while (length-- > 0) {
      size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
      *dest++ = charset[index];
   }
   *dest = '\0';
}


void cleanup_file(char *path)
{
   if (FileExists(path)) {
      printf("Cleaning up: %s\n", path);
      DeleteFile(path);
   }
}

void copy_file(char *from, char *to)
{
   if (!CopyFile(from, to, FALSE)) {
      printf("ERROR: Failed to copy '%s' to '%s'\n", from, to);
      exit(1);
   }
}

void touch_file(char *path)
{
   HANDLE file = CreateFile(
      path, 
      FILE_READ_DATA, FILE_SHARE_READ, 
      NULL, OPEN_ALWAYS, 0,
      NULL
   );

   CloseHandle(file);
}

#endif /* ifndef UTILS_C  */
