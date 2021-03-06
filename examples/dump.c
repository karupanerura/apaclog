#include "apaclog.h"

int main (void) {
  struct apaclog_format *format = apaclog_parse_format("foo bar %%%% %a %{USER}e foo");
  apaclog_dump_format(stdout, format);
  apaclog_free_format(format);
  return 0;
}
