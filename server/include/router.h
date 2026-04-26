#ifndef ROUTER_H
#define ROUTER_H

#include "common.h"
#include "handler.h"

/*
   Looks up a handler for (method, uri) and dispatches it.
   Falls back to a static-file responder for /static/ and /assets/,
   and to 404 / 405 otherwise.

   For HEAD requests, the matching GET handler runs and the body is then
   stripped by the caller; this keeps Content-Length accurate per RFC 7231.
*/
success_flag_t router_dispatch(http_request_t* req, http_response_t* res);

#endif
