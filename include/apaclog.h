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
  char         *remote_addr;
  unsigned int  remote_addr_len;
  char         *filename;
  unsigned int  filename_len;
  char         *remote_host;
  unsigned int  remote_host_len;
  unsigned int  minor_version;
  char         *request_method;
  unsigned int  request_method_len;
  unsigned int  server_port;
  unsigned int  process_id;
  char         *query_string;
  unsigned int  query_string_len;
  char         *remote_user;
  unsigned int  remote_user_len;
  char         *path_info;
  unsigned int  path_info_len;
  char         *server_name;
  unsigned int  server_name_len;
  unsigned int  bytes_received;
  // RESPONSE:
  //   TODO: response_header
  unsigned int  bytes_response;
  unsigned long request_time_usec;
  unsigned int  response_status;
  unsigned long response_time_usec;
  enum apaclog_connection_status connection_status;
  unsigned int  bytes_sent;
  void         *extra;
};

struct apaclog_format_token {
  struct apaclog_format_token *next;
  enum apaclog_format_type type;
  char *str;
  unsigned int strlen;
  void *extra;
};

struct apaclog_modifier {
  void *(*non_quoted_format_parser)(const char type);
  void *(*quoted_format_parser)(const char type);
  void  (*renderer)(struct apaclog_format_token *token, struct apaclog_info *info, FILE *out);
};

struct apaclog_format {
  struct apaclog_format_token *token;
  struct apaclog_modifier     *modifier;
  const char *src;
};

struct apaclog_format *apaclog_parse_format (const char *src);
struct apaclog_format *apaclog_parse_format_custom (const char *src, struct apaclog_modifier *modifier);
void apaclog_dump_format (FILE *out, struct apaclog_format *format);
void apaclog_render_file (FILE *out, struct apaclog_format *format, struct apaclog_info *info);

inline void apaclog_fnputs (const char *str, const unsigned int length, FILE *out) {
  for (unsigned int i = 0; i < length; i++) {
    fputc(str[i], out);
  }
}

inline void apaclog_token_fputs (struct apaclog_format_token *token, FILE *out) {
  apaclog_fnputs(token->str, token->strlen, out);
}

inline struct apaclog_modifier *apaclog_new_modifier() {
  struct apaclog_modifier *modifier = (struct apaclog_modifier *)malloc(sizeof(struct apaclog_modifier));
  modifier->non_quoted_format_parser = NULL;
  modifier->quoted_format_parser     = NULL;
  modifier->renderer                 = NULL;
  return modifier;
}

// FIXME: fix me!!!!!!!!!!!!!!!!!!!!!!!!
inline struct apaclog_info *apaclog_new_info() {
  struct apaclog_info *info = (struct apaclog_info *)malloc(sizeof(struct apaclog_info));
  info->remote_addr         = NULL;
  info->remote_addr_len     = 0;
  info->filename            = NULL;
  info->filename_len        = 0;
  info->remote_host         = NULL;
  info->remote_host_len     = 0;
  info->minor_version       = 0;
  info->request_method      = NULL;
  info->request_method_len  = 0;
  info->server_port         = 0;
  info->process_id          = 0;
  info->query_string        = NULL;
  info->query_string_len    = 0;
  info->remote_user         = NULL;
  info->remote_user_len     = 0;
  info->path_info           = NULL;
  info->path_info_len       = 0;
  info->server_name         = NULL;
  info->server_name_len     = 0;
  info->bytes_received      = 0;
  info->bytes_response      = 0;
  info->request_time_usec   = 0;
  info->response_status     = 0;
  info->response_time_usec  = 0;
  info->bytes_sent          = 0;
  info->extra               = NULL;
  return info;
}

inline void apaclog_free_info(struct apaclog_info *info) {
  if (info->extra != NULL) {
    free(info->extra);
  }
  free(info);
}

inline struct apaclog_format_token *apaclog_new_format_token() {
  struct apaclog_format_token *token = (struct apaclog_format_token *)malloc(sizeof(struct apaclog_format_token));
  token->next   = NULL;
  token->str    = NULL;
  token->strlen = 0;
  token->extra  = NULL;
  return token;
}

inline struct apaclog_format_token *apaclog_new_next_format_token(struct apaclog_format_token *curr) {
  struct apaclog_format_token *next = apaclog_new_format_token();
  curr->next = next;
  return next;
}

inline void apaclog_free_modifier (struct apaclog_modifier *modifier) {
  if (modifier != NULL) {
    free(modifier);
  }
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
  apaclog_free_modifier(format->modifier);
  free(format);
}

#endif
