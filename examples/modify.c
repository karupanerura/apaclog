#include "apaclog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADERS_SIZE 3

enum my_token_type {
  TOKEN_REQUEST_HEADER,
  TOKEN_TIME,
  TOKEN_DOUBLE_QUOTE
};

struct my_token {
  enum my_token_type type;
};

struct my_header {
  const char *name;
  const char *value;
};

struct my_token *new_my_token () {
  return (struct my_token *)malloc(sizeof(struct my_token));
}

void *parse_quoted_format (const char type) {
  struct my_token *token = NULL;
  switch (type) {
    case 'i':
      token = new_my_token();
      token->type = TOKEN_REQUEST_HEADER;
      break;
  }
  return token;
}

void *parse_non_quoted_format (const char type) {
  struct my_token *token = NULL;
  switch (type) {
    case 't': // can override
      token = new_my_token();
      token->type = TOKEN_TIME;
      break;
    case '"': // can use new definition
      token = new_my_token();
      token->type = TOKEN_DOUBLE_QUOTE;
      break;
  }
  return token;
}

void original_token_renderer (struct apaclog_format_token *token, struct apaclog_info *info, FILE *out) {
  struct my_token   *my_token = (struct my_token  *)token->extra;
  struct my_header  *headers  = (struct my_header *)info->extra;
  switch (my_token->type) {
    case TOKEN_REQUEST_HEADER:
      for (int i = 0; i < HEADERS_SIZE; i++) {
        if (strlen(headers[i].name) == token->strlen) {
          if (strncmp(headers[i].name, token->str, token->strlen) == 0) {
            fputs(headers[i].value, out);
            break;
          }
        }
      }
      break;
    case TOKEN_TIME:
      fputs("2014-09-09T00:11:22+0900", out);
      break;
    case TOKEN_DOUBLE_QUOTE:
      fputc('"', out);
      break;
  }
}

int main (void) {
  const char *src = "%h - %u %t %\"%r%\" %s %b \"%{Referer}i\" \"%{User-agent}i\"";

  struct apaclog_modifier *modifier = apaclog_new_modifier();
  modifier->quoted_format_parser     = parse_quoted_format;
  modifier->non_quoted_format_parser = parse_non_quoted_format;
  modifier->renderer                 = original_token_renderer;

  struct apaclog_format *format = apaclog_parse_format_custom(src, modifier);
  {
    // set my headers
    struct my_header *headers = (struct my_header *)malloc(sizeof(struct my_header) * HEADERS_SIZE);
    headers[0].name  = "Referer";
    headers[0].value = "http://www.google.co.jp/";
    headers[1].name  = "User-agent";
    headers[1].value = "MyClient/1.0";
    headers[2].name  = "Host";
    headers[2].value = "foo-bar.net";

    // set request info
    struct apaclog_info *info = apaclog_new_info();
    info->remote_host        = "127.0.0.1";
    info->remote_host_len    = 9;
    info->remote_user        = "anon";
    info->remote_user_len    = 4;
    info->request_method     = "GET";
    info->request_method_len = 3;
    info->path_info          = "/path/to/foo";
    info->path_info_len      = 12;
    info->query_string       = "foo=bar&debug=1";
    info->query_string_len   = 15;
    info->minor_version      = 0;
    info->response_status    = 200;
    info->bytes_response     = 12345;
    info->extra              = headers;

    // render
    apaclog_render_file(stdout, format, info);

    // free (with info->extra)
    apaclog_free_info(info);
    // or free extra manualy
    // free(info->extra);
    // info->extra = NULL;
  };

  // free (with format->modifier)
  apaclog_free_format(format);
  // or free modifier manualy
  // free(format->modifier);
  // format->modifier = NULL;

  return 0;
}
