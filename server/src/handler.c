#include "../include/handler.h"
#include "../include/mime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>


/* ──────────────────────────────────────────────────────────────────────── */
/*  Building blocks                                                         */
/* ──────────────────────────────────────────────────────────────────────── */

static success_flag_t alloc_skeleton(http_response_t* res)
{
    res->res_line = calloc(1, sizeof(status_line_t));
    res->head     = calloc(1, sizeof(headers_list_t));
    res->body     = NULL;
    res->body_size = 0;
    if (!res->res_line || !res->head) return FAILURE;
    strcpy(res->res_line->http_version, "HTTP/1.1");
    return SUCCESS;
}

static success_flag_t respond_buffer(http_response_t* res,
                                     int status, const char* reason,
                                     const char* content_type,
                                     char* body, size_t body_size)
{
    if (alloc_skeleton(res) != SUCCESS) return FAILURE;
    res->res_line->status = (int16_t)status;
    strncpy(res->res_line->reason, reason, REASON_LENGTH - 1);
    if (content_type) header_set(res->head, "Content-Type", content_type);
    res->body = body;
    res->body_size = body_size;
    return SUCCESS;
}

static success_flag_t respond_file(http_response_t* res,
                                   int status, const char* reason,
                                   const char* path)
{
    long size = 0;
    char* body = load_file(path, &size);
    if (!body) return FAILURE;
    return respond_buffer(res, status, reason, mime_for_path(path),
                          body, (size_t)size);
}

success_flag_t make_status_response(http_response_t* res,
                                    int status, const char* reason,
                                    const char* body_text)
{
    size_t n = body_text ? strlen(body_text) : 0;
    char*  copy = NULL;
    if (n > 0) {
        copy = malloc(n + 1);
        if (!copy) return FAILURE;
        memcpy(copy, body_text, n);
        copy[n] = '\0';
    }
    return respond_buffer(res, status, reason,
                          "text/plain; charset=utf-8", copy, n);
}

success_flag_t make_404(http_response_t* res)
{
    /* Try the pretty page first, fall back to plain text. */
    if (respond_file(res, 404, "Not Found", "static/404.html") == SUCCESS)
        return SUCCESS;
    return make_status_response(res, 404, "Not Found", "404 — Not Found\n");
}

success_flag_t make_405(http_response_t* res, const char* allow)
{
    if (make_status_response(res, 405, "Method Not Allowed",
                             "405 — Method Not Allowed\n") != SUCCESS)
        return FAILURE;
    if (allow) header_set(res->head, "Allow", allow);
    return SUCCESS;
}

success_flag_t make_500(http_response_t* res)
{
    return make_status_response(res, 500, "Internal Server Error",
                                "500 — Internal Server Error\n");
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  Page handlers                                                           */
/* ──────────────────────────────────────────────────────────────────────── */

success_flag_t handle_index(http_request_t* req, http_response_t* res)
{
    (void)req;
    if (respond_file(res, 200, "OK", "static/index.html") == SUCCESS) return SUCCESS;
    return make_500(res);
}

success_flag_t handle_about(http_request_t* req, http_response_t* res)
{
    (void)req;
    if (respond_file(res, 200, "OK", "static/about.html") == SUCCESS) return SUCCESS;
    return make_500(res);
}

success_flag_t handle_contact(http_request_t* req, http_response_t* res)
{
    (void)req;
    if (respond_file(res, 200, "OK", "static/contact.html") == SUCCESS) return SUCCESS;
    return make_500(res);
}

success_flag_t handle_form_get(http_request_t* req, http_response_t* res)
{
    (void)req;
    if (respond_file(res, 200, "OK", "static/form.html") == SUCCESS) return SUCCESS;
    return make_500(res);
}


/* Form POST: very small URL-decode, just enough to echo what was sent. */
static void url_decode_inplace(char* s)
{
    char* w = s;
    for (char* r = s; *r; r++) {
        if (*r == '+') { *w++ = ' '; }
        else if (*r == '%' && r[1] && r[2]) {
            char hex[3] = { r[1], r[2], 0 };
            *w++ = (char)strtol(hex, NULL, 16);
            r += 2;
        } else { *w++ = *r; }
    }
    *w = '\0';
}

success_flag_t handle_form_post(http_request_t* req, http_response_t* res)
{
    const char* ctype = header_get(req->head, "Content-Type");
    if (!ctype || strncasecmp(ctype, "application/x-www-form-urlencoded", 33) != 0) {
        return make_status_response(res, 415, "Unsupported Media Type",
                                    "415 — expected application/x-www-form-urlencoded\n");
    }

    char* form = req->body ? strndup(req->body, req->body_size) : strdup("");
    if (!form) return make_500(res);

    /* Build a tiny HTML page echoing the parsed key/value pairs. */
    char* page = malloc(8192);
    if (!page) { free(form); return make_500(res); }

    int off = snprintf(page, 8192,
        "<!doctype html><meta charset=\"utf-8\"><title>Submitted</title>"
        "<style>body{font-family:Cinzel,serif;background:#1a1410;color:#f5f0e8;"
        "padding:3rem;text-align:center}a{color:#c9a96e}"
        "table{margin:2rem auto;border-collapse:collapse}"
        "td{padding:.6rem 1.2rem;border-bottom:1px solid #c9a96e44}"
        ".k{color:#c9a96e;text-align:right}.v{color:#e8dfd0;text-align:left}</style>"
        "<h1>Received</h1><table>");

    char* save = NULL;
    for (char* pair = strtok_r(form, "&", &save);
         pair && off < 8000;
         pair = strtok_r(NULL, "&", &save))
    {
        char* eq = strchr(pair, '=');
        const char* k = pair;
        const char* v = "";
        if (eq) { *eq = '\0'; v = eq + 1; }
        char keybuf[256], valbuf[1024];
        strncpy(keybuf, k, sizeof(keybuf) - 1); keybuf[sizeof(keybuf) - 1] = 0;
        strncpy(valbuf, v, sizeof(valbuf) - 1); valbuf[sizeof(valbuf) - 1] = 0;
        url_decode_inplace(keybuf);
        url_decode_inplace(valbuf);
        off += snprintf(page + off, 8192 - off,
            "<tr><td class=k>%s</td><td class=v>%s</td></tr>", keybuf, valbuf);
    }
    off += snprintf(page + off, 8192 - off,
        "</table><p><a href=\"/form\">← back</a></p>");

    free(form);

    if (alloc_skeleton(res) != SUCCESS) { free(page); return FAILURE; }
    res->res_line->status = 200;
    strcpy(res->res_line->reason, "OK");
    header_set(res->head, "Content-Type", "text/html; charset=utf-8");
    res->body = page;
    res->body_size = (size_t)off;
    return SUCCESS;
}


success_flag_t handle_echo(http_request_t* req, http_response_t* res)
{
    /* Echoes the request body back as JSON (or text, if the body wasn't JSON). */
    const char* ctype = header_get(req->head, "Content-Type");
    if (!ctype) ctype = "text/plain; charset=utf-8";

    size_t n = req->body_size;
    char* copy = malloc(n + 1);
    if (!copy) return make_500(res);
    if (n > 0) memcpy(copy, req->body, n);
    copy[n] = '\0';

    return respond_buffer(res, 200, "OK", ctype, copy, n);
}


success_flag_t handle_time(http_request_t* req, http_response_t* res)
{
    (void)req;
    char date[64];
    http_date_now(date, sizeof(date));

    char* body = malloc(256);
    if (!body) return make_500(res);
    int n = snprintf(body, 256,
        "{\"now\":\"%s\",\"unix\":%ld}\n",
        date, (long)time(NULL));

    return respond_buffer(res, 200, "OK", "application/json",
                          body, (size_t)n);
}


success_flag_t handle_health(http_request_t* req, http_response_t* res)
{
    (void)req;
    char* body = strdup("{\"status\":\"ok\"}\n");
    if (!body) return make_500(res);
    return respond_buffer(res, 200, "OK", "application/json",
                          body, strlen(body));
}


success_flag_t handle_options(http_request_t* req, http_response_t* res)
{
    (void)req;
    if (alloc_skeleton(res) != SUCCESS) return FAILURE;
    res->res_line->status = 204;
    strcpy(res->res_line->reason, "No Content");
    header_set(res->head, "Allow", "GET, HEAD, POST, OPTIONS");
    return SUCCESS;
}


/* Static-file fallback under /static/<path> and /assets/<path>. */
success_flag_t handle_static(http_request_t* req, http_response_t* res)
{
    const char* uri = req->req_line->uri;

    /* Path traversal guard. */
    if (strstr(uri, "..")) return make_404(res);

    char path[URI_LENGTH + 16];
    /* /static/foo  -> static/foo  ;  /assets/bar -> assets/bar */
    if (strncmp(uri, "/static/", 8) == 0) {
        snprintf(path, sizeof(path), "static/%s", uri + 8);
    } else if (strncmp(uri, "/assets/", 8) == 0) {
        snprintf(path, sizeof(path), "assets/%s", uri + 8);
    } else {
        return make_404(res);
    }

    if (respond_file(res, 200, "OK", path) == SUCCESS) return SUCCESS;
    return make_404(res);
}
