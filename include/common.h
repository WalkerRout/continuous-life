#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WINDOWS
  #include <windows.h>
#else
  #include <unistd.h>
  #define Sleep(x) usleep((x)*1000)
#endif
#define sleep Sleep // want to use sleep(...) instead of Sleep(...) to fit conventions

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#endif // COMMON_H