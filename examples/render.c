#include "apaclog.h"
#include <stdio.h>
#include <time.h>

int main (void) {
  const char *src = "%h - %u %{[%d/%m/%Y:%H:%M:%S %z]}t \"%r\" %s %b"; // "%h - %u %t \"%r\" %s %b";
  struct apaclog_format *format = apaclog_parse_format(src);

  struct apaclog_info info;
  time_t    epoch;
  struct tm date;
  {
    // set time
    time(&epoch);
    localtime_r(&epoch, &date);

    // set log info
    apaclog_info_init(&info);
    info.request_date         = &date;
    info.remote_addr          = "127.0.0.1";
    info.remote_addr_len      = 9;
    info.remote_user          = "anon";
    info.remote_user_len      = 4;
    info.request_method       = "GET";
    info.request_method_len   = 3;
    info.path_info            = "/path/to/foo";
    info.path_info_len        = 12;
    info.query_string         = "foo=bar&debug=1";
    info.query_string_len     = 15;
    info.request_protocol     = "HTTP/1.0";
    info.request_protocol_len = 8;
    info.response_status      = 200;
    info.bytes_response       = 12345;

    // render
    apaclog_render_file(stdout, format, &info);
  }
  {
    // set time
    time(&epoch);
    localtime_r(&epoch, &date);

    // set log info
    apaclog_info_init(&info);
    info.request_date         = &date;
    info.remote_addr          = "192.168.0.1";
    info.remote_addr_len      = 11;
    info.remote_user          = "-";
    info.remote_user_len      = 1;
    info.request_method       = "POST";
    info.request_method_len   = 4;
    info.path_info            = "/path/to/bar";
    info.path_info_len        = 12;
    info.query_string         = NULL;
    info.query_string_len     = 0;
    info.request_protocol     = "HTTP/1.0";
    info.request_protocol_len = 8;
    info.response_status      = 404;
    info.bytes_response       = 0;

    // render
    apaclog_render_file(stdout, format, &info);
  }

  apaclog_free_format(format);
  return 0;
}
