#include "../include/serializer.h"
#include <stdio.h>
#include <string.h>


n_bytes_t serialize_req_head(char* buf, size_t cap, const http_request_t* req)
{
    if (!buf || cap == 0 || !req || !req->req_line) return FAILURE;

    size_t off = 0;
    int w;

    w = snprintf(buf + off, cap - off, "%s %s %s\r\n",
                 req->req_line->method,
                 req->req_line->uri,
                 req->req_line->http_version);
    if (w < 0 || (size_t)w >= cap - off) return FAILURE;
    off += (size_t)w;

    if (req->head) {
        for (size_t i = 0; i < req->head->headers_counter; i++) {
            w = snprintf(buf + off, cap - off, "%s: %s\r\n",
                         req->head->headers[i].name,
                         req->head->headers[i].value);
            if (w < 0 || (size_t)w >= cap - off) return FAILURE;
            off += (size_t)w;
        }
    }

    w = snprintf(buf + off, cap - off, "\r\n");
    if (w < 0 || (size_t)w >= cap - off) return FAILURE;
    off += (size_t)w;

    return (n_bytes_t)off;
}


n_bytes_t serialize_res_head(char* buf, size_t cap, const http_response_t* res)
{
    if (!buf || cap == 0 || !res || !res->res_line) return FAILURE;

    size_t off = 0;
    int w;

    w = snprintf(buf + off, cap - off, "%s %d %s\r\n",
                 res->res_line->http_version,
                 res->res_line->status,
                 res->res_line->reason);
    if (w < 0 || (size_t)w >= cap - off) return FAILURE;
    off += (size_t)w;

    if (res->head) {
        for (size_t i = 0; i < res->head->headers_counter; i++) {
            w = snprintf(buf + off, cap - off, "%s: %s\r\n",
                         res->head->headers[i].name,
                         res->head->headers[i].value);
            if (w < 0 || (size_t)w >= cap - off) return FAILURE;
            off += (size_t)w;
        }
    }

    w = snprintf(buf + off, cap - off, "\r\n");
    if (w < 0 || (size_t)w >= cap - off) return FAILURE;
    off += (size_t)w;

    return (n_bytes_t)off;
}


n_bytes_t serialize_req(char* buf, size_t cap, const http_request_t* req)
{
    n_bytes_t head = serialize_req_head(buf, cap, req);
    if (head < 0) return FAILURE;

    if (req->body && req->body_size > 0) {
        if ((size_t)head + req->body_size >= cap) return FAILURE;
        memcpy(buf + head, req->body, req->body_size);
        head = (n_bytes_t)((size_t)head + req->body_size);
    }
    return head;
}
