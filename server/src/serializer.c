#include "../include/serializer.h"

success_flag_t serialize_req(char* buffer, http_request_t* req, size_t buffer_size) {
    size_t offset = 0;
    n_bytes_t written;

    if (!buffer || buffer_size == 0)
        return FAILURE;
    
    buffer[0] = '\0';

    // Request line: METHOD URI VERSION\r\n
    written = snprintf(buffer + offset, buffer_size - offset,
                       "%s %s %s\r\n",
                       req->req_line->method,
                       req->req_line->uri,
                       req->req_line->http_version
                       );
    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;
    offset += written;

    // Headers
    for (size_t i = 0; i < req->head->headers_counter; i++) {
        written = snprintf(buffer + offset, buffer_size - offset,
                           "%s: %s\r\n",
                           req->head->headers[i].name,
                           req->head->headers[i].value
                           );
        if (written < 0 || (size_t)written >= buffer_size - offset)
            return FAILURE;
        offset += written;
    }

    // Empty line between headers and body
    written = snprintf(buffer + offset, buffer_size - offset, "\r\n");
    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;
    offset += written;

    // Body (optional)
    if (req->body && req->body_size > 0) {
        if (req->body_size >= buffer_size - offset)  // Fix 1: correct bounds check before memcpy
            return FAILURE;
        memcpy(buffer + offset, req->body, req->body_size);
        offset += req->body_size;
        buffer[offset] = '\0';                      // Fix 2: null-terminate after body
    }

    return SUCCESS;
}


#include "../include/serializer.h"

success_flag_t serialize_res(char* buffer, http_response_t* res, size_t buffer_size) {
    size_t offset = 0;
    n_bytes_t written;

    if (!buffer || buffer_size == 0)
        return FAILURE;

    buffer[0] = '\0';

    // Status line: VERSION STATUS_CODE REASON\r\n
    written = (n_bytes_t)snprintf(buffer + offset, buffer_size - offset,
                       "%s %d %s\r\n",
                       res->res_line->http_version,
                       res->res_line->status,
                       res->res_line->reason
                       );
    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;
    offset += written;

    // Headers
    for (size_t i = 0; i < res->head->headers_counter; i++) {
        written = snprintf(buffer + offset, buffer_size - offset,
                           "%s: %s\r\n",
                           res->head->headers[i].name,
                           res->head->headers[i].value
                           );
        if (written < 0 || (size_t)written >= buffer_size - offset)
            return FAILURE;
        offset += written;
    }

    // Empty line between headers and body
    written = snprintf(buffer + offset, buffer_size - offset, "\r\n");
    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;
    offset += written;

    // Body (optional)
    if (res->body && res->body_size > 0) {
        if (res->body_size >= buffer_size - offset)
            return FAILURE;
        memcpy(buffer + offset, res->body, res->body_size);
        offset += res->body_size;
        buffer[offset] = '\0';
    }

    return SUCCESS;
}