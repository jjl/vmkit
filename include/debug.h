//===----------------- debug.h - Debug facilities -------------------------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef DEBUG_H
#define DEBUG_H

#define ESC "\033["
#define COLOR_NORMAL ""
#define END "m"

#define _BLACK "30"
#define _RED "31"
#define _GREEN "32"
#define _YELLOW "33"
#define _BLUE "34"
#define _MAGENTA "35"
#define _CYAN "36"
#define _WHITE "37"

#define _NORMAL "00"
#define _BOLD "01"
#define _SOULIGNE "04"

#define MK_COLOR(type, bg) type";"bg

/*
#define MK_COLOR(type, bg, fg) type";"bg";"fg

#define WHITE MK_COLOR(_NORMAL, _BLACK, _BLACK)

#define DARK_MAGENTA MK_COLOR(_NORMAL, _BLACK, _MAGENTA)
#define DARK_YELLOW MK_COLOR(_NORMAL, _BLACK, _YELLOW)
#define DARK_CYAN MK_COLOR(_NORMAL, _BLACK, _CYAN)
#define DARK_BLUE MK_COLOR(_NORMAL, _BLACK, _BLUE)
#define DARK_GREEN MK_COLOR(_NORMAL, _BLACK, _GREEN)

#define LIGHT_MAGENTA MK_COLOR(_BOLD, _BLACK, _MAGENTA)
#define LIGHT_YELLOW MK_COLOR(_BOLD, _BLACK, _YELLOW)
#define LIGHT_CYAN MK_COLOR(_BOLD, _BLACK, _CYAN)
#define LIGHT_BLUE MK_COLOR(_BOLD, _BLACK, _BLUE)
#define LIGHT_GREEN MK_COLOR(_BOLD, _BLACK, _GREEN)
#define LIGHT_RED MK_COLOR(_BOLD, _BLACK, _RED)
*/

#define WHITE MK_COLOR(_NORMAL, _WHITE)

#define DARK_MAGENTA MK_COLOR(_NORMAL, _MAGENTA)
#define DARK_YELLOW MK_COLOR(_NORMAL,  _YELLOW)
#define DARK_CYAN MK_COLOR(_NORMAL, _CYAN)
#define DARK_BLUE MK_COLOR(_NORMAL, _BLUE)
#define DARK_GREEN MK_COLOR(_NORMAL, _GREEN)

#define LIGHT_MAGENTA MK_COLOR(_BOLD, _MAGENTA)
#define LIGHT_YELLOW MK_COLOR(_BOLD, _YELLOW)
#define LIGHT_CYAN MK_COLOR(_BOLD, _CYAN)
#define LIGHT_BLUE MK_COLOR(_BOLD, _BLUE)
#define LIGHT_GREEN MK_COLOR(_BOLD, _GREEN)
#define LIGHT_RED MK_COLOR(_BOLD,  _RED)

#undef DEBUG
#define DEBUG 0

#if DEBUG > 0
	#define WITH_COLOR

  #ifdef WITH_COLOR
    #define PRINT_DEBUG(symb, level, color, ...) \
      if (symb > level) { \
          fprintf(stderr, "%s%s%s", ESC, color, END); \
          fprintf(stderr, __VA_ARGS__); \
          fprintf(stderr, "%s%s%s", ESC, COLOR_NORMAL, END); \
          fflush(stderr);\
      }
  #else
    #define PRINT_DEBUG(symb, level, color, ...) \
      if (symb > level) { \
        fprintf(stderr, __VA_ARGS__);\
		fflush(stderr);\
      }
  #endif

#else
	#define PRINT_DEBUG(symb, level, color, args...) do {} while(0);

#endif

#define UNIMPLEMENTED() {                           \
  vmkit::Thread::get()->printBacktrace();           \
  fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
  abort(); }                                      \

#define ABORT() UNIMPLEMENTED()
#define UNREACHABLE() ABORT()

#define ASSERT(cond) {  \
  if (!cond) ABORT(); } \

#undef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__ ((always_inline))
#define NO_INLINE __attribute__ ((noinline))

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif
#endif

// Silence compiler warnings.
template <typename T>
static inline void USE(T) { }

#endif
