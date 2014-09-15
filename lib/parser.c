#include "apaclog.h"
#include <stdlib.h>

char *_apaclog_capture_format (struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier);
char *_apaclog_capture_quoted_format (struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier);
char *_apaclog_capture_non_quoted_format (struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier);

struct apaclog_format *apaclog_parse_format (const char *src) {
  return apaclog_parse_format_custom(src, NULL);
}

// TODO: care case of malloc failed.
struct apaclog_format *apaclog_parse_format_custom (const char *src, struct apaclog_modifier *modifier) {
  struct apaclog_format *format = apaclog_new_format(src, modifier);

  char *p = (char *)src;
  struct apaclog_format_token *curr = format->token;
  struct apaclog_format_token *last = format->token;
  while (*p != '\0') {
    if (*p == '%') {
      // format
      p = _apaclog_capture_format(curr, ++p, modifier);
      if (p == NULL) {
        apaclog_free_format(format);
        return NULL;
      }
      p++;
    }
    else {
      // raw string
      curr->type = APACLOG_TOKEN_RAW_STRING;
      curr->str  = p;
      while (*p != '%' && *p != '\0') p++;
      curr->strlen = p - curr->str;
    }
    last = curr;
    curr = apaclog_new_next_format_token(curr);
  }
  free(last->next);
  last->next = NULL;
  return format;
}

char *_apaclog_capture_format(struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier) {
  // NULL str check
  if (*p == '\0') {
    return NULL;
  }

  // is quoted format?
  if (*p == '{') {
    // capture args
    token->str = ++p;
    while (*p != '}') {
      // NULL str check
      if (*p == '\0') {
        return NULL;
      }
      p++;
    }
    token->strlen = p++ - token->str;
    return _apaclog_capture_quoted_format(token, p, modifier);
  }
  else {
    // non quoted format
    return _apaclog_capture_non_quoted_format(token, p, modifier);
  }
}

char *_apaclog_capture_quoted_format(struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier) {
  // modify it?
  if (modifier != NULL && modifier->quoted_format_parser != NULL) {
    void *extra = (*modifier->quoted_format_parser)(*p);
    if (extra != NULL) {
      token->type  = APACLOG_TOKEN_USER_DEFINED;
      token->extra = extra;
      return p;
    }
  }

  // capture type
  switch (*p) {
    case 'C':
      token->type = APACLOG_TOKEN_COOKIE;
      break;
    case 'e':
      token->type = APACLOG_TOKEN_ENV;
      break;
    case 'i':
      token->type = APACLOG_TOKEN_REQUEST_HEADER;
      break;
    case 'o':
      token->type = APACLOG_TOKEN_RESPONSE_HEADER;
      break;
    case 't':
      token->type = APACLOG_TOKEN_REQUEST_DATE_STRFTIME;
      break;
    default:
      return NULL;
  }

  return p;
}

char *_apaclog_capture_non_quoted_format (struct apaclog_format_token *token, char *p, struct apaclog_modifier *modifier) {
  // modify it?
  if (modifier != NULL && modifier->non_quoted_format_parser != NULL) {
    void *extra = (*modifier->non_quoted_format_parser)(*p);
    if (extra != NULL) {
      token->type  = APACLOG_TOKEN_USER_DEFINED;
      token->extra = extra;
      return p;
    }
  }

  // capture type
  switch (*p) {
    case '%':
      token->type   = APACLOG_TOKEN_RAW_STRING;
      token->str    = "%";
      token->strlen = 1;
      break;
    case 'a':
      token->type = APACLOG_TOKEN_REMOTE_ADDR;
      break;
    case 'B':
      token->type = APACLOG_TOKEN_BYTES_RESPONSE;
      break;
    case 'b':
      token->type = APACLOG_TOKEN_BYTES_RESPONSE_CLF;
      break;
    case 'D':
      token->type = APACLOG_TOKEN_REQUEST_TIME_USEC;
      break;
    case 'f':
      token->type = APACLOG_TOKEN_FILENAME;
      break;
    case 'h':
      token->type = APACLOG_TOKEN_REMOTE_HOST;
      break;
    case 'H':
      token->type = APACLOG_TOKEN_REQUEST_PROTOCOL;
      break;
    case 'm':
      token->type = APACLOG_TOKEN_REQUEST_METHOD;
      break;
    case 'p':
      token->type = APACLOG_TOKEN_SERVER_PORT;
      break;
    case 'P':
      token->type = APACLOG_TOKEN_PROCESS_ID;
      break;
    case 'q':
      token->type = APACLOG_TOKEN_QUERY_STRING;
      break;
    case 'r':
      token->type = APACLOG_TOKEN_REQUEST_FIRST_LINE;
      break;
    case 's':
      token->type = APACLOG_TOKEN_RESPONSE_STATUS;
      break;
    case 't':
      token->type = APACLOG_TOKEN_REQUEST_DATE_CLF;
      break;
    case 'T':
      token->type = APACLOG_TOKEN_RESPONSE_TIME_SEC;
      break;
    case 'u':
      token->type = APACLOG_TOKEN_REMOTE_USER;
      break;
    case 'U':
      token->type = APACLOG_TOKEN_PATH_INFO;
      break;
    case 'v':
      token->type = APACLOG_TOKEN_SERVER_NAME;
      break;
    case 'X':
      token->type = APACLOG_TOKEN_CONNECTION_STATUS;
      break;
    case 'I':
      token->type = APACLOG_TOKEN_BYTES_RECEIVED;
      break;
    case 'O':
      token->type = APACLOG_TOKEN_BYTES_SENT;
      break;
    default:
      return NULL;
  }

  return p;
}
