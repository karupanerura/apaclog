#include "apaclog.h"
#include <stdio.h>
#include <unistd.h>

void apaclog_dump_format (FILE *out, struct apaclog_format *format) {
  fprintf(out, "dump_apaclog_format:\n");
  fprintf(out, "  src:   %s\n", format->src);
  fprintf(out, "  token: [\n");

  int i = 0;
  struct apaclog_format_token *token = format->token;
  while (token != NULL) {
    fprintf(out, "    [%d]\n", i);
    fprintf(out, "      id:%d\n", token->type);
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
        // TODO
        fputc('-', out);
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
      case APACLOG_TOKEN_RESPONSE_STATUS:
        fprintf(out, "%u", info->response_status);
        break;
      case APACLOG_TOKEN_REQUEST_DATE_CLF:
        // TODO
        fputc('-', out);
        break;
      case APACLOG_TOKEN_REQUEST_DATE_STRFTIME:
        // TODO
        fputc('-', out);
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
    }
    token = token->next;
  }
}
