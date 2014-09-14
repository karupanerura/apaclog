#include "apaclog.h"

int main (void) {
  const char *src = "%h - %u %t \"%r\" %s %b \"%{Referer}i\" \"%{User-agent}i\"";
  struct apaclog_format *format = apaclog_parse_format(src);
  {
    struct apaclog_info info = {
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
    struct apaclog_info info = {
      .remote_host        = "127.0.0.1",
      .remote_host_len    = 8,
      .remote_user        = NULL,
      .remote_user_len    = 0,
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
  apaclog_free_format(format);
  return 0;
}
