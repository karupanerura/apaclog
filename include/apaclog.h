#ifndef apaclog_h
#define apaclog_h
#include <stdio.h>
#include <stdlib.h>

enum apaclog_format_type {
  APACLOG_TOKEN_RAW_STRING,               // raw string.
  APACLOG_TOKEN_REMOTE_ADDR,              // %a
  APACLOG_TOKEN_BYTES_RESPONSE,           // %B
  APACLOG_TOKEN_BYTES_RESPONSE_CLF,       // %b
  APACLOG_TOKEN_COOKIE,                   // %{VARNAME}C
  APACLOG_TOKEN_REQUEST_TIME_USEC,        // %D
  APACLOG_TOKEN_ENV,                      // %{VARNAME}e
  APACLOG_TOKEN_FILENAME,                 // %f
  APACLOG_TOKEN_REMOTE_HOST,              // %h
  APACLOG_TOKEN_REQUEST_PROTOCOL,         // %H
  APACLOG_TOKEN_REQUEST_HEADER,           // %{VARNAME}i
  APACLOG_TOKEN_REQUEST_METHOD,           // %m
  APACLOG_TOKEN_RESPONSE_HEADER,          // %{VERNAME}o
  APACLOG_TOKEN_SERVER_PORT,              // %p
  APACLOG_TOKEN_PROCESS_ID,               // %P
  APACLOG_TOKEN_QUERY_STRING,             // %q
  APACLOG_TOKEN_RESPONSE_STATUS,          // %s
  APACLOG_TOKEN_REQUEST_DATE_CLF,         // %t
  APACLOG_TOKEN_REQUEST_DATE_STRFTIME,    // %{format}t
  APACLOG_TOKEN_RESPONSE_TIME_SEC,        // %T
  APACLOG_TOKEN_REMOTE_USER,              // %u
  APACLOG_TOKEN_PATH_INFO,                // %U
  APACLOG_TOKEN_SERVER_NAME,              // %v
  APACLOG_TOKEN_CONNECTION_STATUS,        // %X
  APACLOG_TOKEN_BYTES_RECEIVED,           // %I
  APACLOG_TOKEN_BYTES_SENT                // %O
};

struct apaclog_format_token {
  struct apaclog_format_token *next;
  enum apaclog_format_type type;
  char *str;
  unsigned int strlen;
};

struct apaclog_format {
  struct apaclog_format_token *token;
  const char *src;
};

// for %X(CONNECTION_STATUS)
enum apaclog_connection_status {
  APACLOG_STATUS_ABORTED,    // X
  APACLOG_STATUS_KEEP_ALIVE, // +
  APACLOG_STATUS_CLOSED      // -
};

// FIXME: TOO LARGE
struct apaclog_info {
  // REQUEST:
  //   TODO: cookie
  //   TODO: request_header
  //   TODO: request_date
  const char         *remote_addr;
  const unsigned int  remote_addr_len;
  const char         *filename;
  const unsigned int  filename_len;
  const char         *remote_host;
  const unsigned int  remote_host_len;
  const unsigned int  minor_version;
  const char         *request_method;
  const unsigned int  request_method_len;
  const unsigned int  server_port;
  const unsigned int  process_id;
  const char         *query_string;
  const unsigned int  query_string_len;
  const char         *remote_user;
  const unsigned int  remote_user_len;
  const char         *path_info;
  const unsigned int  path_info_len;
  const char         *server_name;
  const unsigned int  server_name_len;
  const unsigned int  bytes_received;
  // RESPONSE:
  //   TODO: response_header
  const unsigned int  bytes_response;
  const unsigned long request_time_usec;
  const unsigned int  response_status;
  const unsigned long response_time_usec;
  const enum apaclog_connection_status connection_status;
  const unsigned int  bytes_sent;
};

struct apaclog_format *apaclog_parse_format (const char *src);
void apaclog_dump_format (FILE *out, struct apaclog_format *format);
void apaclog_render_file (FILE *out, struct apaclog_format *format, struct apaclog_info *info);

inline void apaclog_fnputs (const char *str, const unsigned int length, FILE *out) {
  for (int i = 0; i < length; i++) {
    fputc(str[i], out);
  }
}

inline void apaclog_token_fputs (struct apaclog_format_token *token, FILE *out) {
  apaclog_fnputs(token->str, token->strlen, out);
}

inline void apaclog_free_format (struct apaclog_format *format) {
  struct apaclog_format_token *token = format->token;
  while (token != NULL) {
    struct apaclog_format_token *next = token->next;
    free(token);
    token = next;
  }
  free(format);
}

#endif
