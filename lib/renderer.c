#include "apaclog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define APACLOG_RENDERER_MAX_BUFFER_SIZE  1023
#define APACLOG_RENDERER_MAX_ENVNAME_SIZE 63
#define APACLOG_RENDERER_MAX_STRFMT_SIZE  63

int apaclog_render_env(char *p, const size_t size, struct apaclog_format_token *token);
int apaclog_render_clftime(char *p, const size_t size, struct apaclog_format_token *token, struct tm *timeptr);
int apaclog_render_strftime(char *p, const size_t size, struct apaclog_format_token *token, struct tm *timeptr);

void apaclog_dump_format (FILE *out, struct apaclog_format *format) {
  fprintf(out, "dump_apaclog_format:\n");
  fprintf(out, "  src:   %s\n", format->src);
  fprintf(out, "  token: [\n");

  int i = 0;
  struct apaclog_format_token *token = format->token;
  while (token != NULL) {
    fprintf(out, "    [%d]\n", i);
    fprintf(out, "      type_id:%d\n", token->type);
    fprintf(out, "      strlen:%d\n", token->strlen);
    if (token->str != NULL) {
      fprintf(out, "      str:");
      apaclog_token_fputs(token, out);
      fputc('\n', out);
    }
    token = token->next;
    i++;
  }
  fprintf(out, "  ]\n");
}

int apaclog_render_file (FILE *out, struct apaclog_format *format, struct apaclog_info *info) {
  static char buf[APACLOG_RENDERER_MAX_BUFFER_SIZE];

  // render to buffer
  char *ret = apaclog_render_buf(buf, APACLOG_RENDERER_MAX_BUFFER_SIZE, format, info);

  // is failed?
  if (ret == NULL) {
    return -1;
  }

  // render to file
  int len = fputs(buf, out);
  fflush(out);
  return len;
}

char *apaclog_render_buf (char *buf, const size_t max, struct apaclog_format *format, struct apaclog_info *info) {
  int diff;
  char *p = buf;
  size_t size = (size_t)max;
  struct apaclog_format_token *token = format->token;

#define PUTC2BUF(c) {  \
    if (size == 0) {   \
      return NULL;     \
    }                  \
    *p = c;            \
    p++;               \
    size--;            \
  }

#define PUSH2BUF(str, strlen) {    \
    if ((strlen) > size) {         \
      return NULL;                 \
    }                              \
    strncpy(p, (str), (strlen));   \
    p += (strlen);                 \
    size -= (strlen);              \
  }

#define SPRINTF2BUF(fmt, __VA_ARGS__) {            \
    diff = snprintf(p, size, fmt, __VA_ARGS__);  \
    if (diff < 0) {                                \
      return NULL;                                 \
    }                                              \
    p += diff;                                     \
    size -= diff;                                  \
  }

  while (token != NULL) {
    switch (token->type) {
      case APACLOG_TOKEN_RAW_STRING:
        PUSH2BUF(token->str, token->strlen);
        break;
      case APACLOG_TOKEN_REMOTE_ADDR:
        PUSH2BUF(info->remote_addr, info->remote_addr_len);
        break;
      case APACLOG_TOKEN_BYTES_RESPONSE:
        SPRINTF2BUF("%u", info->bytes_response);
        break;
      case APACLOG_TOKEN_BYTES_RESPONSE_CLF:
        if (info->bytes_response) {
          SPRINTF2BUF("%u", info->bytes_response);
        } else {
          PUTC2BUF('-');
        }
        break;
      case APACLOG_TOKEN_REQUEST_TIME_USEC:
        SPRINTF2BUF("%lu", info->request_time_usec);
        break;
      case APACLOG_TOKEN_ENV:
        diff = apaclog_render_env(p, size, token);
        if (diff < 0) {
          return NULL;
        }
        p += diff;
        size -= diff;
        break;
      case APACLOG_TOKEN_FILENAME:
        PUSH2BUF(info->filename, info->filename_len);
        break;
      case APACLOG_TOKEN_REMOTE_HOST:
        PUSH2BUF(info->remote_addr, info->remote_addr_len);
        break;
      case APACLOG_TOKEN_REQUEST_PROTOCOL:
        PUSH2BUF(info->request_protocol, info->request_protocol_len);
        break;
      case APACLOG_TOKEN_REQUEST_METHOD:
        PUSH2BUF(info->request_method, info->request_method_len);
        break;
      case APACLOG_TOKEN_SERVER_PORT:
        SPRINTF2BUF("%u", info->server_port);
        break;
      case APACLOG_TOKEN_PROCESS_ID:
        SPRINTF2BUF("%u", getpid());
        break;
      case APACLOG_TOKEN_QUERY_STRING:
        PUSH2BUF(info->query_string, info->query_string_len);
        break;
      case APACLOG_TOKEN_REQUEST_FIRST_LINE:
        PUSH2BUF(info->request_method, info->request_method_len);
        PUTC2BUF(' ');
        PUSH2BUF(info->path_info, info->path_info_len);
        if (info->query_string != NULL && info->query_string_len > 0) {
          PUTC2BUF('?');
          PUSH2BUF(info->query_string, info->query_string_len);
        }
        PUTC2BUF(' ');
        PUSH2BUF(info->request_protocol, info->request_protocol_len);
        break;
      case APACLOG_TOKEN_RESPONSE_STATUS:
        SPRINTF2BUF("%u", info->response_status);
        break;
      case APACLOG_TOKEN_REQUEST_DATE_CLF:
        if (info->request_date != NULL) {
          diff = apaclog_render_clftime(p, size, token, info->request_date);
          if (diff < 0) {
            return NULL;
          }
          p += diff;
          size -= diff;
        }
        else {
          PUTC2BUF('-');
        }
        break;
      case APACLOG_TOKEN_REQUEST_DATE_STRFTIME:
        if (info->request_date != NULL) {
          diff = apaclog_render_strftime(p, size, token, info->request_date);
          if (diff < 0) {
            return NULL;
          }
          p += diff;
          size -= diff;
        }
        else {
          PUTC2BUF('-');
        }
        break;
      case APACLOG_TOKEN_RESPONSE_TIME_SEC:
        SPRINTF2BUF("%Lf", ((long double)info->request_time_usec) / 1000000.0);
        break;
      case APACLOG_TOKEN_REMOTE_USER:
        PUSH2BUF(info->remote_user, info->remote_user_len);
        break;
      case APACLOG_TOKEN_PATH_INFO:
        PUSH2BUF(info->path_info, info->path_info_len);
        break;
      case APACLOG_TOKEN_SERVER_NAME:
        PUSH2BUF(info->server_name, info->server_name_len);
        break;
      case APACLOG_TOKEN_CONNECTION_STATUS:
        switch (info->connection_status) {
          case APACLOG_STATUS_ABORTED:
            PUTC2BUF('X');
            break;
          case APACLOG_STATUS_KEEP_ALIVE:
            PUTC2BUF('+');
            break;
          case APACLOG_STATUS_CLOSED:
            PUTC2BUF('-');
            break;
        }
        break;
      case APACLOG_TOKEN_BYTES_RECEIVED:
        SPRINTF2BUF("%u", info->bytes_received);
        break;
      case APACLOG_TOKEN_BYTES_SENT:
        SPRINTF2BUF("%u", info->bytes_sent);
        break;
      case APACLOG_TOKEN_USER_DEFINED:
        diff = (*format->modifier->renderer)(format->modifier, p, size, token, info);
        if (diff < 0) {
          return NULL;
        }
        p += diff;
        size -= diff;
        break;
    }
    token = token->next;
  }
  PUTC2BUF('\n');
  PUTC2BUF('\0');

  return buf;
}

int apaclog_render_env(char *p, const size_t size, struct apaclog_format_token *token) {
  static char name[APACLOG_RENDERER_MAX_ENVNAME_SIZE];
  static char *value;
  static size_t length;

  // set name
  strncpy(name, token->str, token->strlen);
  name[token->strlen] = '\0';

  // set value
  value  = getenv(name);
  length = strlen(value);

  // size over?
  if (size < length) {
    return -1;
  }
  strcpy(p, value);

  return size - length;
}

int apaclog_render_clftime(char *p, const size_t size, struct apaclog_format_token *token, struct tm *timeptr) {
  const static char month2str[12][4] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
  };
  const long gmtoff_abs = labs(timeptr->tm_gmtoff);

  return snprintf(p, size,
    "[%02d/%s/%04d:%02d:%02d:%02d %c%02ld%02ld]",
    timeptr->tm_mday,
    month2str[timeptr->tm_mon],
    1900 + timeptr->tm_year,
    timeptr->tm_hour,
    timeptr->tm_min,
    timeptr->tm_sec,
    timeptr->tm_gmtoff > 0 ? '+' : '-',
    gmtoff_abs / 3600,
    (gmtoff_abs % 3600) / 60
  );
}

int apaclog_render_strftime(char *p, const size_t size, struct apaclog_format_token *token, struct tm *timeptr) {
  static char format[APACLOG_RENDERER_MAX_STRFMT_SIZE];
  static size_t length;
  strncpy(format, token->str, token->strlen);
  format[token->strlen] = '\0';
  length = strftime(p, size, format, timeptr);
  return (length == 0) ? -1 : length;
}
