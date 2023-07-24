#include <assert.h>
#include <stdio.h>

#include <MchlkrpchLogger/logger.h>
#include <stdarg.h>

struct Logger logger = {1, 0, "", NULL};

/// Main pointer to unique logger reference.
extern struct Logger *logger_ptr = &logger;

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
void __TabIncr()
{
  for (uint64_t cur_sym = logger_ptr->tab_size * logger_ptr->n_tabs;
    cur_sym < logger_ptr->tab_size * (logger_ptr->n_tabs + 1); ++cur_sym) {
    logger_ptr->indent[cur_sym] = (cur_sym % 2 == 1)? ' ' : '\'';
  }

  ++logger_ptr->n_tabs;
}

/// @brief Decreases 'logger_ptr->n_tabs'. 
void __TabDecr()
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
  // Prefix message contains: message status, time, split pipe-symbol.
  char msg_prefix[512] = "";
  
  switch (style) {
    case W: { sprintf(msg_prefix, "%s", "" YELLOW_FMT "WRN " WHITE_FMT __TIME__ "| "); break; }
    case E: { sprintf(msg_prefix, "%s", "" RED_FMT    "ERR " WHITE_FMT __TIME__ "| "); break; }
    case M: { sprintf(msg_prefix, "%s", "" WHITE_FMT  "MSG " WHITE_FMT __TIME__ "| "); break; }
    default:{}
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
void DebugPrint(const char style, const char *fmt, ...)
{
  if (fmt == NULL) {
    return;
    // printf("fmt:\"%s\"\n", fmt);
  }

  WriteStatusFile(style);

  va_list args;
  va_start(args, fmt);

  char *msg = va_arg(args, char*);
  // printf("buff:%s", msg);

  char msg_prefix[512] = "";
  sprintf(msg_prefix, fmt, msg);

  if (logger_ptr->f == NULL) {
    printf("%s", msg_prefix);
  } else {
    fprintf(logger_ptr->f, "%s", msg_prefix);
  }
}

