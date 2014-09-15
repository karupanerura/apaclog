#ifndef apaclog_util_h
#define apaclog_util_h

#include <stdio.h>
#include <stdlib.h>

inline void apaclog_fnputs (const char *str, const unsigned int length, FILE *out) {
  for (unsigned int i = 0; i < length; i++) {
    fputc(str[i], out);
  }
}

inline void *apaclog_malloc(size_t sz) {
  void *p = malloc(sz);
  if (p == NULL) {
    perror("no memory");
    abort();
  }
  return p;
}

#endif
