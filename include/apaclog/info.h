#ifndef apaclog_info_h
#define apaclog_info_h

#include <stdlib.h>
#include "connection_status.h"

// FIXME: TOO LARGE
struct apaclog_info {
  // REQUEST:
  struct tm    *request_date;
  char         *remote_addr;
  unsigned int  remote_addr_len;
  char         *filename;
  unsigned int  filename_len;
  char         *request_protocol;
  unsigned int  request_protocol_len;
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
  unsigned int  bytes_response;
  unsigned long request_time_usec;
  unsigned int  response_status;
  unsigned long response_time_usec;
  enum apaclog_connection_status connection_status;
  unsigned int  bytes_sent;
  void         *extra;
};

// FIXME: fix me!!!!!!!!!!!!!!!!!!!!!!!!
inline struct apaclog_info *apaclog_info_init(struct apaclog_info *info) {
  info->request_date         = NULL;
  info->remote_addr          = NULL;
  info->remote_addr_len      = 0;
  info->filename             = NULL;
  info->filename_len         = 0;
  info->request_protocol     = NULL;
  info->request_protocol_len = 0;
  info->request_method       = NULL;
  info->request_method_len   = 0;
  info->server_port          = 0;
  info->process_id           = 0;
  info->query_string         = NULL;
  info->query_string_len     = 0;
  info->remote_user          = NULL;
  info->remote_user_len      = 0;
  info->path_info            = NULL;
  info->path_info_len        = 0;
  info->server_name          = NULL;
  info->server_name_len      = 0;
  info->bytes_received       = 0;
  info->bytes_response       = 0;
  info->request_time_usec    = 0;
  info->response_status      = 0;
  info->response_time_usec   = 0;
  info->bytes_sent           = 0;
  info->extra                = NULL;
  return info;
}

inline struct apaclog_info *apaclog_new_info() {
  struct apaclog_info *info = (struct apaclog_info *)apaclog_malloc(sizeof(struct apaclog_info));
  return apaclog_info_init(info);
}

inline void apaclog_free_info(struct apaclog_info *info) {
  if (info->extra != NULL) {
    free(info->extra);
  }
  free(info);
}

#endif
