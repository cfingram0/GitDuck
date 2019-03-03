#include "windows.h"
#include <cassert>
#include <cstdio>

void WriteToOutput(char * str, ...) {
   char buff[1024] = {};

   assert(strlen(str) < 800);

   va_list arg;
   va_start(arg, str);
   _vsprintf_p(buff, sizeof(buff), str, arg);
   va_end(arg);

   OutputDebugString(buff);
}