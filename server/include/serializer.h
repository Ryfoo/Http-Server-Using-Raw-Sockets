#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stddef.h>
#include "common.h"

/*
   Builds the request-line + headers + CRLF into `head_buffer`.
   Returns the number of bytes written, or FAILURE.
   The caller is responsible for sending req->body separately.
*/
n_bytes_t serialize_req_head(char* head_buffer, size_t head_buffer_size,
                             const http_request_t* req);

/*
   Builds the status-line + headers + CRLF into `head_buffer`.
   Returns the number of bytes written, or FAILURE.
   The caller is responsible for sending res->body separately, using the
   Content-Length the handler attached.
*/
n_bytes_t serialize_res_head(char* head_buffer, size_t head_buffer_size,
                             const http_response_t* res);

/* Convenience kept for the existing client driver. Builds the full request
   into one buffer. Returns number of bytes written or FAILURE. */
n_bytes_t serialize_req(char* buffer, size_t buffer_size,
                        const http_request_t* req);

#endif
