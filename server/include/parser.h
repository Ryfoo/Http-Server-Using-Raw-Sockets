#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "common.h"

/*
   parse_req_head
   --------------
   Parses the request-line and headers from `head` (a NUL-terminated string
   that ends right after the first CRLFCRLF). Body is NOT touched here —
   the socket layer reads it after consulting Content-Length.

   On SUCCESS: req->req_line and req->head are freshly malloc()'d and owned
   by the caller. req->body is initialised to NULL / 0.
*/
success_flag_t parse_req_head(const char* head, http_request_t* req);

success_flag_t parse_res(const char* raw, http_response_t* res);

#endif
