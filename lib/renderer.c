#include "apaclog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void apaclog_render_env(char *buf, struct apaclog_format_token *token);
void apaclog_render_strftime(char *buf, struct apaclog_format_token *token, struct tm *timeptr);
void apaclog_render_clftime(char *buf, struct apaclog_format_token *token, struct tm *timeptr);

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

void apaclog_render_file (FILE *out, struct apaclog_format *format, struct apaclog_info *info) {
  static char buf[APACLOG_RENDERER_MAX_BUFFER_SIZE];

  struct apaclog_format_token *token = format->token;
  while (token != NULL) {
    switch (token->type) {
      case APACLOG_TOKEN_RAW_STRING:
        apaclog_token_fputs(token, out);
        break;
      case APACLOG_TOKEN_REMOTE_ADDR:
        apaclog_fnputs(info->remote_addr, info->remote_addr_len, out);
        break;
      case APACLOG_TOKEN_BYTES_RESPONSE:
        fprintf(out, "%u", info->bytes_response);
        break;
      case APACLOG_TOKEN_BYTES_RESPONSE_CLF:
        if (info->bytes_response) {
          fprintf(out, "%u", info->bytes_response);
        } else {
          fputc('-', out);
        }
        break;
      case APACLOG_TOKEN_COOKIE:
        // TODO
        fputc('-', out);
        break;
      case APACLOG_TOKEN_REQUEST_TIME_USEC:
        fprintf(out, "%lu", info->request_time_usec);
        break;
      case APACLOG_TOKEN_ENV:
        apaclog_render_env(buf, token);
        fputs(buf, out);
        break;
      case APACLOG_TOKEN_FILENAME:
        apaclog_fnputs(info->filename, info->filename_len, out);
        break;
      case APACLOG_TOKEN_REMOTE_HOST:
        apaclog_fnputs(info->remote_host, info->remote_host_len, out);
        break;
      case APACLOG_TOKEN_REQUEST_PROTOCOL:
        fprintf(out, "HTTP/1.%u", info->minor_version);
        break;
      case APACLOG_TOKEN_REQUEST_HEADER:
        // TODO
        fputc('-', out);
        break;
      case APACLOG_TOKEN_REQUEST_METHOD:
        apaclog_fnputs(info->request_method, info->request_method_len, out);
        break;
      case APACLOG_TOKEN_RESPONSE_HEADER:
        // TODO
        fputc('-', out);
        break;
      case APACLOG_TOKEN_SERVER_PORT:
        fprintf(out, "%u", info->server_port);
        break;
      case APACLOG_TOKEN_PROCESS_ID:
        fprintf(out, "%u", getpid());
        break;
      case APACLOG_TOKEN_QUERY_STRING:
        apaclog_fnputs(info->query_string, info->query_string_len, out);
        break;
      case APACLOG_TOKEN_REQUEST_FIRST_LINE:
        apaclog_fnputs(info->request_method, info->request_method_len, out);
        fputc(' ', out);
        apaclog_fnputs(info->path_info, info->path_info_len, out);
        if (info->query_string != NULL && info->query_string_len > 0) {
          fputc('?', out);
          apaclog_fnputs(info->query_string, info->query_string_len, out);
        }
        fprintf(out, " HTTP/1.%u", info->minor_version);
        break;
      case APACLOG_TOKEN_RESPONSE_STATUS:
        fprintf(out, "%u", info->response_status);
        break;
      case APACLOG_TOKEN_REQUEST_DATE_CLF:
        if (info->request_date != NULL) {
          apaclog_render_clftime(buf, token, info->request_date);
          fputs(buf, out);
        }
        else {
          fputc('-', out);
        }
        break;
      case APACLOG_TOKEN_REQUEST_DATE_STRFTIME:
        if (info->request_date != NULL) {
          apaclog_render_strftime(buf, token, info->request_date);
          fputs(buf, out);
        }
        else {
          fputc('-', out);
        }
        break;
      case APACLOG_TOKEN_RESPONSE_TIME_SEC:
        fprintf(out, "%Lf", ((long double)info->request_time_usec) / 1000000.0);
        break;
      case APACLOG_TOKEN_REMOTE_USER:
        apaclog_fnputs(info->remote_user, info->remote_user_len, out);
        break;
      case APACLOG_TOKEN_PATH_INFO:
        apaclog_fnputs(info->path_info, info->path_info_len, out);
        break;
      case APACLOG_TOKEN_SERVER_NAME:
        apaclog_fnputs(info->server_name, info->server_name_len, out);
        break;
      case APACLOG_TOKEN_CONNECTION_STATUS:
        switch (info->connection_status) {
          case APACLOG_STATUS_ABORTED:
            fputc('X', out);
            break;
          case APACLOG_STATUS_KEEP_ALIVE:
            fputc('+', out);
            break;
          case APACLOG_STATUS_CLOSED:
            fputc('-', out);
            break;
        }
        break;
      case APACLOG_TOKEN_BYTES_RECEIVED:
        fprintf(out, "%u", info->bytes_received);
        break;
      case APACLOG_TOKEN_BYTES_SENT:
        fprintf(out, "%u", info->bytes_sent);
        break;
      case APACLOG_TOKEN_USER_DEFINED:
        (*format->modifier->renderer)(token, info, out);
        break;
    }
    token = token->next;
  }
  fputc('\n', out);
}

void apaclog_render_env(char *buf, struct apaclog_format_token *token) {
  static char name[APACLOG_RENDERER_MAX_BUFFER_SIZE];
  static char *value;

  // set name
  strncpy(name, token->str, token->strlen);
  name[token->strlen] = '\0';

  // set value
  value = getenv(name);
  strcpy(buf, value);
}

void apaclog_render_clftime(char *buf, struct apaclog_format_token *token, struct tm *timeptr) {
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

  snprintf(buf, APACLOG_RENDERER_MAX_BUFFER_SIZE,
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

void apaclog_render_strftime(char *buf, struct apaclog_format_token *token, struct tm *timeptr) {
  static char format[APACLOG_RENDERER_MAX_BUFFER_SIZE];
  strncpy(format, token->str, token->strlen);
  format[token->strlen] = '\0';
  strftime(buf, APACLOG_RENDERER_MAX_BUFFER_SIZE, format, timeptr);
}
