#pragma once

#include <stdint.h>

#include <MchlkrpchLogger/loggerGroups.h>

#define YELLOW_FMT "\033[0;33m"
#define    RED_FMT "\033[0;31m"
#define  WHITE_FMT "\033[0;37m"

#define W 'W'
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

void __TabIncr();

void __TabDecr();

void WriteStatusConsole(const char style);

void WriteStatusFile(const char style);

void DebugPrint(const char style, const char *fmt, ...);

#define __msg(x, msg_style, fmt, ...)                                                                \
  DebugPrint(msg_style, ((x & chosen_group) != 0x0) ? logger_ptr->indent : NULL, ##__VA_ARGS__);      \
  DebugPrint(' ',       ((x & chosen_group) != 0x0) ? fmt                : NULL, ##__VA_ARGS__);
