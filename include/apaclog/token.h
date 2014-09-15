#ifndef apaclog_token_h
#define apaclog_token_h

#include <stdlib.h>
#include "util.h"

enum apaclog_format_type {
  APACLOG_TOKEN_RAW_STRING,               // raw string.
  APACLOG_TOKEN_REMOTE_ADDR,              // %a
  APACLOG_TOKEN_BYTES_RESPONSE,           // %B
  APACLOG_TOKEN_BYTES_RESPONSE_CLF,       // %b
  // APACLOG_TOKEN_COOKIE,                // %{VARNAME}C
  APACLOG_TOKEN_REQUEST_TIME_USEC,        // %D
  APACLOG_TOKEN_ENV,                      // %{VARNAME}e
  APACLOG_TOKEN_FILENAME,                 // %f
  APACLOG_TOKEN_REMOTE_HOST,              // %h
  APACLOG_TOKEN_REQUEST_PROTOCOL,         // %H
  // APACLOG_TOKEN_REQUEST_HEADER,        // %{VARNAME}i
  APACLOG_TOKEN_REQUEST_METHOD,           // %m
  // APACLOG_TOKEN_RESPONSE_HEADER,       // %{VERNAME}o
  APACLOG_TOKEN_SERVER_PORT,              // %p
  APACLOG_TOKEN_PROCESS_ID,               // %P
  APACLOG_TOKEN_QUERY_STRING,             // %q
  APACLOG_TOKEN_REQUEST_FIRST_LINE,       // %r
  APACLOG_TOKEN_RESPONSE_STATUS,          // %s
  APACLOG_TOKEN_REQUEST_DATE_CLF,         // %t
  APACLOG_TOKEN_REQUEST_DATE_STRFTIME,    // %{format}t
  APACLOG_TOKEN_RESPONSE_TIME_SEC,        // %T
  APACLOG_TOKEN_REMOTE_USER,              // %u
  APACLOG_TOKEN_PATH_INFO,                // %U
  APACLOG_TOKEN_SERVER_NAME,              // %v
  APACLOG_TOKEN_CONNECTION_STATUS,        // %X
  APACLOG_TOKEN_BYTES_RECEIVED,           // %I
  APACLOG_TOKEN_BYTES_SENT,               // %O
  APACLOG_TOKEN_USER_DEFINED              // user defined token.
};

struct apaclog_format_token {
  struct apaclog_format_token *next;
  enum apaclog_format_type type;
  char *str;
  unsigned int strlen;
  void *extra;
};

inline struct apaclog_format_token *apaclog_format_token_init(struct apaclog_format_token *token) {
  token->next   = NULL;
  token->str    = NULL;
  token->strlen = 0;
  token->extra  = NULL;
  return token;
}

inline struct apaclog_format_token *apaclog_new_format_token() {
  struct apaclog_format_token *token = (struct apaclog_format_token *)apaclog_malloc(sizeof(struct apaclog_format_token));
  return apaclog_format_token_init(token);
}

inline struct apaclog_format_token *apaclog_new_next_format_token(struct apaclog_format_token *curr) {
  struct apaclog_format_token *next = apaclog_new_format_token();
  curr->next = next;
  return next;
}

inline void apaclog_token_fputs (struct apaclog_format_token *token, FILE *out) {
  apaclog_fnputs(token->str, token->strlen, out);
}

#endif
