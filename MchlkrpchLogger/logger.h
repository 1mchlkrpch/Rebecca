#pragma once

#include <stdint.h>

#include <MchlkrpchLogger/loggerGroups.h>

// Colored text in console.
#define YELLOW_FMT "\033[0;33m"
#define    RED_FMT "\033[0;31m"
#define  WHITE_FMT "\033[0;37m"
#define   BLUE_FMT "\033[0;34m"
#define  GREEN_FMT "\033[5;32m"

static const size_t kSpecialIndentSymbolMod = 5;

// Modes of output.
#define W 'W'
#define C 'C'
#define E 'E'
#define M 'M'

/// @brief Main structure of logger.
struct Logger
{
  // Tab size in spaces.
  uint64_t tab_size;
  // Number of tabs in message.
  uint64_t n_tabs;
  /*Indent string to insert
  it before every message.*/
  char indent[256];
  // Logger file.
  FILE *f;
};

extern struct Logger *logger_ptr;

void SetTabSize(uint64_t n_spaces);

void SetLogfile(FILE *f);

void __tab_incr();

void __tab_decr();

void WriteStatusConsole(const char style);

void WriteStatusFile(const char style);

void DebugPrint(const char style, char *fmt, ...);

#define __spt(msg_group_name)                                                                                                 \
  for (size_t i = 0; i < 5; ++i) {                                                                                             \
    DebugPrint(M,   ((msg_group_name & chosen_group) != 0x0) ? "\n" : NULL);                                                    \
  }

#define __msg(msg_group_name, msg_style, fmt, ...)                                                                                    \
  DebugPrint(msg_style, ((msg_group_name & chosen_group) != 0x0 && msg_style != ' ') ? logger_ptr->indent : NULL, ##__VA_ARGS__);      \
  DebugPrint(' ',       ((msg_group_name & chosen_group) != 0x0) ? fmt                : NULL, ##__VA_ARGS__);

// #define __if_msg(expr, msg_group_name, msg_style, fmt, ...) 
//   ((expr) == true)? __msg(msg_group_name, msg_style, fmt, ##__VA_ARGS__) :; 


#define __asrt(expr, fmt, ...)                                                                                     \
  assert(expr && fmt);

  // ((expr) == true)?                                                                                                 
  //     DebugPrint(C,   ((D_ASSERTS & chosen_group) != 0x0)? logger_ptr->indent : NULL, ##__VA_ARGS__)                 
  //   : DebugPrint(E,   logger_ptr->indent, ##__VA_ARGS__);                                                       
  // ((expr) == true)?                                                                                              
  //     DebugPrint(' ', ((D_ASSERTS & chosen_group) != 0x0)? #expr "- Currect\n" : NULL, ##__VA_ARGS__)             
  //   : DebugPrint(' ', #expr ", "fmt, ##__VA_ARGS__);                                                               
  // ((expr) != true)?                                                                                                 
  //   exit(0) : (void)fmt ;
