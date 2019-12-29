/**
 * Logging interface.
 *
 * Use macros, not the log_* functions directly. ASSERT for compile-time
 * checks. The logging implmentation can be assumed to be thread-safe.
 *
 * Log level guidelines:
 * - die() to log message and terminate.
 * - error() to log conditions where functionality is permanently compromised.
 * - warning() to log conditions where functionality may be temporarily compromised.
 * - info() to log status messages during normal operation.
 * - debug() to log detailed messages that are normally not needed.
 *
 * @author  Mandar Chitre
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <stdint.h>
#include <complex.h>

#define LOG_NONE        0
#define LOG_ERRORS      1
#define LOG_WARNINGS    2
#define LOG_INFO        3
#define LOG_DEBUG       4
#define LOG_ALL         5

#define ASSERT _Static_assert

#ifdef _LOG_MALLOC_

void* log_malloc(const char* module, int line, size_t size);
void* log_calloc(const char* module, int line, size_t count, size_t size);
void  log_free(const char* module, int line, void *ptr);

#define MALLOC(x)       log_malloc(__FILE__, __LINE__, x)
#define CALLOC(x, y)    log_calloc(__FILE__, __LINE__, x, y)
#define FREE(x)         log_free(__FILE__, __LINE__, x)

#else

#define MALLOC(x)       malloc(x)
#define CALLOC(x, y)    calloc(x, y)
#define FREE(x)         free(x)

#endif

#define die(format, ...)      log_die(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define error(format, ...)    log_error(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define warning(format, ...)  log_warning(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define info(format, ...)     log_info(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define debug(format, ...)    log_debug(__FILE__, __LINE__, format, ##__VA_ARGS__)

int   log_open(const char* filename, int maxfiles);
int   log_close(void);

int   log_level(int lvl);

void  log_die(const char* module, int line, const char *format, ...);
int   log_error(const char* module, int line, const char *format, ...);
int   log_warning(const char* module, int line, const char *format, ...);
void  log_info(const char* module, int line, const char *format, ...);
void  log_debug(const char* module, int line, const char *format, ...);

char* bits2str(const uint8_t* data, int len, char* str);
int   dump_passband_signal(const char* filename, const int32_t* signal, int len);
int   dump_baseband_signal(const char* filename, const float complex* signal, int len);

#endif
