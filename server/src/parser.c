#include "../include/parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


static success_flag_t parse_headers(char* cursor, char* saveptr,
                                    headers_list_t* hl)
{
    char* line;
    hl->headers_counter = 0;

    while ((line = strtok_r(cursor, "\r\n", &saveptr)) != NULL) {
        cursor = NULL;
        if (*line == '\0') break;
        if (hl->headers_counter >= HEADERS_LEN) break;

        char* colon = strchr(line, ':');
        if (!colon) continue;

        *colon = '\0';
        char* value = colon + 1;
        while (*value == ' ' || *value == '\t') value++;

        header_t* h = &hl->headers[hl->headers_counter++];
        strncpy(h->name,  line,  HEADER_NAME_LENGTH  - 1);
        h->name[HEADER_NAME_LENGTH - 1] = '\0';
        strncpy(h->value, value, HEADER_VALUE_LENGTH - 1);
        h->value[HEADER_VALUE_LENGTH - 1] = '\0';
    }
    return SUCCESS;
}


success_flag_t parse_req_head(const char* head, http_request_t* req)
{
    if (!head || !req) return FAILURE;

    /* Work on a private copy so strtok_r can mutate it. */
    char* dup = strdup(head);
    if (!dup) return FAILURE;

    char* saveptr = NULL;
    char* line = strtok_r(dup, "\r\n", &saveptr);
    if (!line) { free(dup); return FAILURE; }

    req->req_line = calloc(1, sizeof(request_line_t));
    if (!req->req_line) { free(dup); return FAILURE; }

    /* Request-line: METHOD SP URI SP HTTP-VERSION */
    if (sscanf(line, "%31s %511s %15s",
               req->req_line->method,
               req->req_line->uri,
               req->req_line->http_version) != 3)
    {
        free(req->req_line); req->req_line = NULL;
        free(dup);
        return FAILURE;
    }

    req->head = calloc(1, sizeof(headers_list_t));
    if (!req->head) {
        free(req->req_line); req->req_line = NULL;
        free(dup);
        return FAILURE;
    }

    parse_headers(NULL, saveptr, req->head);

    req->body = NULL;
    req->body_size = 0;

    free(dup);
    return SUCCESS;
}


success_flag_t parse_res(const char* raw, http_response_t* res)
{
    if (!raw || !res) return FAILURE;

    char* dup = strdup(raw);
    if (!dup) return FAILURE;

    char* saveptr = NULL;
    char* line = strtok_r(dup, "\r\n", &saveptr);
    if (!line) { free(dup); return FAILURE; }

    res->res_line = calloc(1, sizeof(status_line_t));
    if (!res->res_line) { free(dup); return FAILURE; }

    int status = 0;
    if (sscanf(line, "%15s %d %63[^\r\n]",
               res->res_line->http_version,
               &status,
               res->res_line->reason) < 2)
    {
        free(res->res_line); res->res_line = NULL;
        free(dup);
        return FAILURE;
    }
    res->res_line->status = (int16_t)status;

    res->head = calloc(1, sizeof(headers_list_t));
    if (!res->head) {
        free(res->res_line); res->res_line = NULL;
        free(dup);
        return FAILURE;
    }
    parse_headers(NULL, saveptr, res->head);

    /* Body, if any. We rely on Content-Length when present. */
    char* body_start = saveptr;
    res->body = NULL;
    res->body_size = 0;

    if (body_start && *body_start) {
        size_t cl = 0;
        for (size_t i = 0; i < res->head->headers_counter; i++) {
            if (strcasecmp(res->head->headers[i].name, "Content-Length") == 0) {
                cl = (size_t)atoi(res->head->headers[i].value);
                break;
            }
        }
        if (cl == 0) cl = strlen(body_start);

        res->body = malloc(cl + 1);
        if (res->body) {
            memcpy(res->body, body_start, cl);
            res->body[cl] = '\0';
            res->body_size = cl;
        }
    }

    free(dup);
    return SUCCESS;
}
