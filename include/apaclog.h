#ifndef apaclog_h
#define apaclog_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "apaclog/token.h"
#include "apaclog/info.h"
#include "apaclog/modifier.h"

struct apaclog_format {
  struct apaclog_format_token *token;
  struct apaclog_modifier     *modifier;
  const char *src;
};

inline struct apaclog_format *apaclog_format_init(struct apaclog_format *format, const char *src, struct apaclog_modifier *modifier) {
  format->src      = src;
  format->token    = apaclog_new_format_token();
  format->modifier = modifier;
  return format;
}

inline struct apaclog_format *apaclog_new_format(const char *src, struct apaclog_modifier *modifier) {
  struct apaclog_format *format = (struct apaclog_format *)apaclog_malloc(sizeof(struct apaclog_format));
  return apaclog_format_init(format, src, modifier);
}

inline void apaclog_free_format (struct apaclog_format *format) {
  struct apaclog_format_token *token = format->token;
  while (token != NULL) {
    struct apaclog_format_token *next = token->next;
    if (token->extra != NULL) {
      free(token->extra);
    }
    free(token);
    token = next;
  }
  if (format->modifier != NULL) {
    apaclog_free_modifier(format->modifier);
  }
  free(format);
}

struct apaclog_format *apaclog_parse_format (const char *src);
struct apaclog_format *apaclog_parse_format_custom (const char *src, struct apaclog_modifier *modifier);
void apaclog_dump_format (FILE *out, struct apaclog_format *format);
int apaclog_render_file (FILE *out, struct apaclog_format *format, struct apaclog_info *info);
char *apaclog_render_buf (char *buf, const size_t max, struct apaclog_format *format, struct apaclog_info *info);

#endif
