/**
 * Logging implementation.
 *
 * @author  Mandar Chitre
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/time.h>
#include "logging.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int level = LOG_INFO;

static inline uint64_t current_time_ms() {
  struct timeval te;
  gettimeofday(&te, NULL);
  return te.tv_sec*1000ll + te.tv_usec/1000;
}

static int rotate_logs(const char* pattern, int maxfiles) {
  int maxlen = strlen(pattern)+8;
  char* buf = MALLOC(2*maxlen);
  if (buf == NULL) return -1;
  char* filename1 = buf;
  char* filename2 = buf + maxlen;
  for (int i = maxfiles-1; i >= 0; i--) {
    sprintf(filename1, pattern, i);
    sprintf(filename2, pattern, i+1);
    rename(filename1, filename2);
  }
  FREE(buf);
  return 0;
}

int log_open(const char* filename, int maxfiles) {
  if (maxfiles > 1) {
    char* pattern = CALLOC(1, strlen(filename)+8);
    if (pattern == NULL) return warning("Out of memory");
    char* p = strstr(filename, "-0.");
    if (p != NULL) {
      memcpy(pattern, filename, p-filename);
      strcat(pattern, "-%d.");
      strcat(pattern, p+3);
      rotate_logs(pattern, maxfiles);
    }
    FREE(pattern);
  }
  if (freopen(filename, "a", stderr) == NULL) return warning("Cannot open log file");
  return 0;
}

int log_close(void) {
  fclose(stderr);
  return 0;
}

int log_level(int lvl) {
  if (lvl >= LOG_NONE && lvl <= LOG_ALL) level = lvl;
  return level;
}

void log_die(const char* module, int line, const char *format, ...) {
  if (level > LOG_NONE) {
    const char* smodule = strrchr(module, '/') ? strrchr(module, '/') + 1 : module;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "%" PRIu64 "|ABORT|%s:%d|", current_time_ms(), smodule, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&mutex);
    va_end(args);
  }
  exit(1);
}

int log_error(const char* module, int line, const char *format, ...) {
  if (level >= LOG_ERRORS) {
    const char* smodule = strrchr(module, '/') ? strrchr(module, '/') + 1 : module;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "%" PRIu64 "|ERROR|%s:%d|", current_time_ms(), smodule, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&mutex);
    va_end(args);
  }
  return -1;
}

int log_warning(const char* module, int line, const char *format, ...) {
  if (level >= LOG_WARNINGS) {
    const char* smodule = strrchr(module, '/') ? strrchr(module, '/') + 1 : module;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "%" PRIu64 "|WARNING|%s:%d|", current_time_ms(), smodule, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&mutex);
    va_end(args);
  }
  return -1;
}

void log_info(const char* module, int line, const char *format, ...) {
  if (level >= LOG_INFO) {
    const char* smodule = strrchr(module, '/') ? strrchr(module, '/') + 1 : module;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "%" PRIu64 "|INFO|%s:%d|", current_time_ms(), smodule, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&mutex);
    va_end(args);
  }
}

void log_debug(const char* module, int line, const char *format, ...) {
  if (level >= LOG_DEBUG) {
    const char* smodule = strrchr(module, '/') ? strrchr(module, '/') + 1 : module;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "%" PRIu64 "|DEBUG|%s:%d|", current_time_ms(), smodule, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    pthread_mutex_unlock(&mutex);
    va_end(args);
  }
}

char* bits2str(const uint8_t* data, int len, char* str) {
  if (str == NULL) {
    str = CALLOC(1, 2*len+1);
    if (str == NULL) return NULL;
  }
  for (int i = 0; i < len; i++)
    sprintf(str+2*i, "%02x", data[i]);
  return str;
}

int dump_baseband_signal(const char* filename, const float complex* signal, int len) {
  FILE* fp = fopen(filename,"wt");
  if (fp == NULL) return -1;
  for (int i = 0; i < len; i++)
    fprintf(fp, "%f,%f\n", creal(signal[i]), cimag(signal[i]));
  fclose(fp);
  return 0;
}

int dump_passband_signal(const char* filename, const int32_t* signal, int len) {
  FILE* fp = fopen(filename,"wt");
  if (fp == NULL) return -1;
  for (int i = 0; i < len; i++)
    fprintf(fp, "%d\n", (int)signal[i]);
  fclose(fp);
  return 0;
}

#ifdef _LOG_MALLOC_

void* log_malloc(const char* module, int line, size_t size) {
  void* p = malloc(size);
  log_info(module, line, "MEM:MALLOC %p (%d bytes)", p, size);
  return p;
}

void* log_calloc(const char* module, int line, size_t count, size_t size) {
  void* p = calloc(count, size);
  log_info(module, line, "MEM:CALLOC %p (%d x %d bytes)", p, count, size);
  return p;
}

void log_free(const char* module, int line, void *p) {
  log_info(module, line, "MEM:FREE %p", p);
  free(p);
}

#endif
