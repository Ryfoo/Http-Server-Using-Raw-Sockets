#include "../include/parser.h"


success_flag_t parse_req(const char *raw, http_request_t *req) {
    if (!raw || !req) return FAILURE;

    char *line, *saveptr;
    
    // Request line
    line = strtok_r((char*) raw,"\r\n", &saveptr);
    if (!line) return FAILURE;

    req->req_line = malloc(sizeof(request_line_t));
    if (!req->req_line) return FAILURE;

    if (sscanf(line, "%31s %255s %15s",
               req->req_line->method,
               req->req_line->uri,
               req->req_line->http_version) != 3) 
    {
        free(req->req_line);
        return FAILURE;
    }

    // Headers
    req->head = malloc(sizeof(headers_list_t));
    if (!req->head) {
        free(req->req_line);
        return FAILURE;
    }
    req->head->headers_counter = 0;

    while ((line = strtok_r(NULL, "\r\n", &saveptr)) && strlen(line) > 0) {
        if (req->head->headers_counter >= HEADERS_LEN) break;

        char *colon = strchr(line, ':');
        if (!colon) continue;

        *colon = '\0';  // split name/value
        colon++;

        // trim leading space
        while (*colon == ' ') colon++;

        header_t *hdr = &req->head->headers[req->head->headers_counter++];
        strncpy(hdr->name, line, sizeof(hdr->name)-1); hdr->name[sizeof(hdr->name)-1] = '\0';
        strncpy(hdr->value, colon, sizeof(hdr->value)-1); hdr->value[sizeof(hdr->value)-1] = '\0';
    }

    // Body
    char *body_start = saveptr;
    if (body_start && *body_start) {
        req->body_size = strlen(body_start);
        req->body = malloc(req->body_size + 1);
        if (!req->body) {
            free(req->head); free(req->req_line);
            return FAILURE;
        }
        strcpy(req->body, body_start);
    } else {
        req->body = NULL;
        req->body_size = 0;
    }

    return SUCCESS;
}


#include "../include/parser.h"

success_flag_t parse_res(const char *raw, http_response_t *res) {
    if (!raw || !res) return FAILURE;

    char *line, *saveptr;

    // Status line
    line = strtok_r((char*)raw, "\r\n", &saveptr);
    if (!line) return FAILURE;

    res->res_line = malloc(sizeof(status_line_t));
    if (!res->res_line) return FAILURE;

    if (sscanf(line, "%31s %d %31s",
                res->res_line->http_version,
                (int *)&res->res_line->status,
                res->res_line->reason) != 3)
    {
        free(res->res_line);
        return FAILURE;
    }

    // Headers
    res->head = malloc(sizeof(headers_list_t));
    if (!res->head) {
        free(res->res_line);
        return FAILURE;
    }
    res->head->headers_counter = 0;

    while ((line = strtok_r(NULL, "\r\n", &saveptr)) && strlen(line) > 0) {
        if (res->head->headers_counter >= HEADERS_LEN) break;

        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = '\0';
        colon++;
        while (*colon == ' ') colon++;

        header_t *hdr = &res->head->headers[res->head->headers_counter++];
        strncpy(hdr->name, line, sizeof(hdr->name)-1);   hdr->name[sizeof(hdr->name)-1]   = '\0';
        strncpy(hdr->value, colon, sizeof(hdr->value)-1); hdr->value[sizeof(hdr->value)-1] = '\0';
    }

    // Body
    char *body_start = saveptr;
    if (body_start && *body_start) {
        // prefer Content-Length if available
        res->body_size = 0;
        for (size_t i = 0; i < res->head->headers_counter; i++) {
            if (strcasecmp(res->head->headers[i].name, "Content-Length") == 0) {
                res->body_size = (size_t)atoi(res->head->headers[i].value);
                break;
            }
        }
        // fallback to strlen
        if (res->body_size == 0)
            res->body_size = strlen(body_start);

        res->body = malloc(res->body_size + 1);
        if (!res->body) {
            free(res->head);
            free(res->res_line);
            return FAILURE;
        }
        memcpy(res->body, body_start, res->body_size);
        res->body[res->body_size] = '\0';
    } else {
        res->body      = NULL;
        res->body_size = 0;
    }

    return SUCCESS;
}