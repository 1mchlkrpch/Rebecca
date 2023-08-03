#include <assert.h>
#include <stdio.h>

#include <MchlkrpchLogger/logger.h>
#include <stdarg.h>

struct Logger logger = {1, 0, "", NULL};

/// Main pointer to unique logger reference.
extern struct Logger *logger_ptr;
struct Logger *logger_ptr = &logger;

/**
 * @brief set new value to tab_size for new change
 * of 'indent' strint.
 * 
 * It is recommended to set it once while programm
 * is executing.
 * 
 * @param n_spaces   Number of space in one tab.
 */
void SetTabSize(uint64_t n_spaces)
{
  assert(n_spaces < 10);
  logger_ptr->tab_size = n_spaces;
}

void SetLogfile(FILE *f)
{
  assert(f != NULL && "Null param");
  logger_ptr->f = f;
}

/// @brief Increases 'logger_ptr->n_tabs'. 
void tab_incr()
{
  for (uint64_t cur_sym = logger_ptr->tab_size * logger_ptr->n_tabs;
    cur_sym < logger_ptr->tab_size * (logger_ptr->n_tabs + 1); ++cur_sym) {
    logger_ptr->indent[cur_sym] = (cur_sym % kSpecialIndentSymbolMod == kSpecialIndentSymbolMod / 2)? '.' : ' ';
  }

  ++logger_ptr->n_tabs;
}

/// @brief Decreases 'logger_ptr->n_tabs'. 
void tab_decr()
{
  if (logger_ptr->n_tabs == 0) {
    return;
  }

  for (uint64_t cur_sym = logger_ptr->tab_size * logger_ptr->n_tabs;
    cur_sym > logger_ptr->tab_size * (logger_ptr->n_tabs - 1); --cur_sym) {
    logger_ptr->indent[cur_sym - 1] = '\0';
  }

  --logger_ptr->n_tabs;
}

/**
 * @brief Writes message prefix
 * to needed output stream.
 * 
 * @param style   Style type (warning, error, common message).
 */
void WriteStatusFile(const char style)
{
  //  Prefix message contains:
  // message status, time, split pipe-symbol.
  char msg_prefix[512] = "";

  char *cursor = msg_prefix;
  uint64_t written = 0;

  if (logger_ptr->f == NULL) {
    switch (style) {
      case C: { written += sprintf(cursor + written, "%s",  GREEN_FMT); break; }
      case W: { written += sprintf(cursor + written, "%s", YELLOW_FMT); break; }
      case E: { written += sprintf(cursor + written, "%s",    RED_FMT); break; }
      case M: { written += sprintf(cursor + written, "%s",  WHITE_FMT); break; }
      default:{}
    }
  }

  switch (style) {
    case C: { written += sprintf(cursor + written, "%s", "NCE "); break; }
    case W: { written += sprintf(cursor + written, "%s", "WRN "); break; }
    case E: { written += sprintf(cursor + written, "%s", "ERR "); break; }
    case M: { written += sprintf(cursor + written, "%s", "MSG "); break; }
    default:{}
  }

  if (logger_ptr->f == NULL) {
    switch (style) {
      case C: { written += sprintf(cursor + written, "%s", BLUE_FMT); break; }
      case W: { written += sprintf(cursor + written, "%s", BLUE_FMT); break; }
      case E: { written += sprintf(cursor + written, "%s", BLUE_FMT); break; }
      case M: { written += sprintf(cursor + written, "%s", BLUE_FMT); break; }
      default:{}
    }
  }

  switch (style) {
    case C: { written += sprintf(cursor + written, "%s", __TIME__ "| "); break; }
    case W: { written += sprintf(cursor + written, "%s", __TIME__ "| "); break; }
    case E: { written += sprintf(cursor + written, "%s", __TIME__ "| "); break; }
    case M: { written += sprintf(cursor + written, "%s", __TIME__ "| "); break; }
    default:{}
  }
  

  if (logger_ptr->f == NULL) {
    switch (style) {
      case C: { written += sprintf(cursor + written, "%s", WHITE_FMT); break; }
      case W: { written += sprintf(cursor + written, "%s", WHITE_FMT); break; }
      case E: { written += sprintf(cursor + written, "%s", WHITE_FMT); break; }
      case M: { written += sprintf(cursor + written, "%s", WHITE_FMT); break; }
      default:{}
    }
  }

  if (logger_ptr->f != NULL) {
    fprintf(logger_ptr->f, "%s", msg_prefix);
  } else {
    printf("%s", msg_prefix);
  }
}

/**
 * @brief Prints message 'msg' to logfile or
 * stdout if it's set.
 * 
 * @param msg   message to print.
 */
void DebugPrint(const char style, char *fmt, ...)
{
  if (fmt == NULL) {
    return;
  }

  WriteStatusFile(style);

  va_list args;
  va_start(args, fmt);

  if (logger_ptr->f == NULL) {
    vprintf(fmt, args);
  } else {
    vfprintf(logger_ptr->f, fmt, args);
  }

  va_end(args);
}

