#include "apaclog.h"
#include <stdlib.h>

inline struct apaclog_format_token *apaclog_new_format_token() {
  struct apaclog_format_token *token = (struct apaclog_format_token *)malloc(sizeof(struct apaclog_format_token));
  token->next   = NULL;
  token->str    = NULL;
  token->strlen = 0;
  return token;
}

inline struct apaclog_format_token *apaclog_new_next_format_token(struct apaclog_format_token *curr) {
  struct apaclog_format_token *next = apaclog_new_format_token();
  curr->next = next;
  return next;
}

// TODO: care case of malloc failed.
struct apaclog_format *apaclog_parse_format (const char *src) {
  struct apaclog_format *format = (struct apaclog_format *)malloc(sizeof(struct apaclog_format));
  format->src   = src;
  format->token = apaclog_new_format_token();

  char *p = (char *)src;
  struct apaclog_format_token *curr = format->token;
  struct apaclog_format_token *last = format->token;
  while (*p != '\0') {
    if (*p == '%') {
      // next character
      p++;

      // NULL str check
      if (*p == '\0') {
        apaclog_free_format(format);
        return NULL;
      }

      // is quoted format?
      if (*p == '{') {
        // capture quoted string
        curr->str = ++p;
        while (*p != '}') {
          // NULL str check
          if (*p == '\0') {
            apaclog_free_format(format);
            return NULL;
          }
          p++;
        }
        curr->strlen = p - curr->str;

        // capture type
        switch (*++p) {
          case 'C':
            curr->type = APACLOG_TOKEN_COOKIE;
            break;
          case 'e':
            curr->type = APACLOG_TOKEN_ENV;
            break;
          case 'i':
            curr->type = APACLOG_TOKEN_REQUEST_HEADER;
            break;
          case 'o':
            curr->type = APACLOG_TOKEN_RESPONSE_HEADER;
            break;
          case 't':
            curr->type = APACLOG_TOKEN_REQUEST_DATE_STRFTIME;
            break;
          default:
            apaclog_free_format(format);
            return NULL;
        }
        p++;
      }
      else {
        // not quoted format
        curr->str    = NULL;
        curr->strlen = 0;
        switch (*p) {
          case '%':
            curr->type   = APACLOG_TOKEN_RAW_STRING;
            curr->str    = "%";
            curr->strlen = 1;
            break;
          case 'a':
            curr->type = APACLOG_TOKEN_REMOTE_ADDR;
            break;
          case 'B':
            curr->type = APACLOG_TOKEN_BYTES_RESPONSE;
            break;
          case 'b':
            curr->type = APACLOG_TOKEN_BYTES_RESPONSE_CLF;
            break;
          case 'D':
            curr->type = APACLOG_TOKEN_REQUEST_TIME_USEC;
            break;
          case 'f':
            curr->type = APACLOG_TOKEN_FILENAME;
            break;
          case 'h':
            curr->type = APACLOG_TOKEN_REMOTE_HOST;
            break;
          case 'H':
            curr->type = APACLOG_TOKEN_REQUEST_PROTOCOL;
            break;
          case 'm':
            curr->type = APACLOG_TOKEN_REQUEST_METHOD;
            break;
          case 'p':
            curr->type = APACLOG_TOKEN_SERVER_PORT;
            break;
          case 'P':
            curr->type = APACLOG_TOKEN_PROCESS_ID;
            break;
          case 'q':
            curr->type = APACLOG_TOKEN_QUERY_STRING;
            break;
          case 'r':
            // alias for "%m %U?%q %H"
            // %m
            curr->type = APACLOG_TOKEN_REQUEST_METHOD;
            curr = apaclog_new_next_format_token(curr);
            // space
            curr->type   = APACLOG_TOKEN_RAW_STRING;
            curr->str    = " ";
            curr->strlen = 1;
            // %U
            curr = apaclog_new_next_format_token(curr);
            curr->type  = APACLOG_TOKEN_PATH_INFO;
            // ?
            curr = apaclog_new_next_format_token(curr);
            curr->type   = APACLOG_TOKEN_RAW_STRING;
            curr->str    = "?";
            curr->strlen = 1;
            // %q
            curr = apaclog_new_next_format_token(curr);
            curr->type  = APACLOG_TOKEN_QUERY_STRING;
            // space
            curr = apaclog_new_next_format_token(curr);
            curr->type   = APACLOG_TOKEN_RAW_STRING;
            curr->str    = " ";
            curr->strlen = 1;
            // %H
            curr = apaclog_new_next_format_token(curr);
            curr->type  = APACLOG_TOKEN_REQUEST_PROTOCOL;
            break;
          case 's':
            curr->type = APACLOG_TOKEN_RESPONSE_STATUS;
            break;
          case 't':
            curr->type = APACLOG_TOKEN_REQUEST_DATE_CLF;
            break;
          case 'T':
            curr->type = APACLOG_TOKEN_RESPONSE_TIME_SEC;
            break;
          case 'u':
            curr->type = APACLOG_TOKEN_REMOTE_USER;
            break;
          case 'U':
            curr->type = APACLOG_TOKEN_PATH_INFO;
            break;
          case 'v':
            curr->type = APACLOG_TOKEN_SERVER_NAME;
            break;
          case 'X':
            curr->type = APACLOG_TOKEN_CONNECTION_STATUS;
            break;
          case 'I':
            curr->type = APACLOG_TOKEN_BYTES_RECEIVED;
            break;
          case 'O':
            curr->type = APACLOG_TOKEN_BYTES_SENT;
            break;
          default:
            apaclog_free_format(format);
            return NULL;
        }
        p++;
      }
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
