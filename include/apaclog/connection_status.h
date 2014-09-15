#ifndef apaclog_connection_status_h
#define apaclog_connection_status_h

// for %X(CONNECTION_STATUS)
enum apaclog_connection_status {
  APACLOG_STATUS_ABORTED,    // X
  APACLOG_STATUS_KEEP_ALIVE, // +
  APACLOG_STATUS_CLOSED      // -
};

#endif
