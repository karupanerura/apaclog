#include "apaclog.h"
#include <stdio.h>
#include <time.h>

int main (void) {
  const char *src = "%h - %u %t \"%r\" %s %b \"%{Referer}i\" \"%{User-agent}i\"";
  struct apaclog_format *format = apaclog_parse_format(src);
  {
    time_t    epoch;
    struct tm date;
    time(&epoch);
    localtime_r(&epoch, &date);

    struct apaclog_info info = {
      .request_date       = &date,
      .remote_host        = "127.0.0.1",
      .remote_host_len    = 9,
      .remote_user        = "anon",
      .remote_user_len    = 4,
      .request_method     = "GET",
      .request_method_len = 3,
      .path_info          = "/path/to/foo",
      .path_info_len      = 12,
      .query_string       = "foo=bar&debug=1",
      .query_string_len   = 15,
      .minor_version      = 0,
      .response_status    = 200,
      .bytes_response     = 12345
    };
    apaclog_render_file(stdout, format, &info);
  }
  {
    time_t    epoch;
    struct tm date;
    time(&epoch);
    localtime_r(&epoch, &date);

    struct apaclog_info info = {
      .request_date       = &date,
      .remote_host        = "192.168.0.1",
      .remote_host_len    = 11,
      .remote_user        = "-",
      .remote_user_len    = 1,
      .request_method     = "POST",
      .request_method_len = 4,
      .path_info          = "/path/to/bar",
      .path_info_len      = 12,
      .query_string       = NULL,
      .query_string_len   = 0,
      .minor_version      = 1,
      .response_status    = 404,
      .bytes_response     = 0
    };
    apaclog_render_file(stdout, format, &info);
  }
  apaclog_free_format(format);
  return 0;
}
