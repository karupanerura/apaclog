#ifndef apaclog_modifier_h
#define apaclog_modifier_h

#include <stdlib.h>
#include "util.h"
#include "info.h"
#include "token.h"

struct apaclog_modifier {
  void *(*non_quoted_format_parser)(struct apaclog_modifier *modifier, const char type);
  void *(*quoted_format_parser)(struct apaclog_modifier *modifier, const char type);
  int   (*renderer)(struct apaclog_modifier *modifier, char *p, const size_t size, struct apaclog_format_token *token, struct apaclog_info *info);
  void *extra;
};

inline struct apaclog_modifier *apaclog_modifier_init(struct apaclog_modifier *modifier) {
  modifier->non_quoted_format_parser = NULL;
  modifier->quoted_format_parser     = NULL;
  modifier->renderer                 = NULL;
  modifier->extra                    = NULL;
  return modifier;
}

inline struct apaclog_modifier *apaclog_new_modifier() {
  struct apaclog_modifier *modifier = (struct apaclog_modifier *)apaclog_malloc(sizeof(struct apaclog_modifier));
  return apaclog_modifier_init(modifier);
}

inline void apaclog_free_modifier (struct apaclog_modifier *modifier) {
  if (modifier->extra != NULL) {
    free(modifier->extra);
  }
  free(modifier);
}

#endif
