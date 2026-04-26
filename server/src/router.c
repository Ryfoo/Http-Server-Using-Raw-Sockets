#include "../include/router.h"
#include <string.h>


typedef success_flag_t (*route_fn)(http_request_t*, http_response_t*);

typedef struct {
    const char* method;   /* exact match */
    const char* path;     /* exact match for now */
    route_fn    handler;
} route_t;


static const route_t ROUTES[] = {
    { "GET",  "/",          handle_index    },
    { "GET",  "/index",     handle_index    },
    { "GET",  "/about",     handle_about    },
    { "GET",  "/contact",   handle_contact  },
    { "GET",  "/form",      handle_form_get },
    { "POST", "/form",      handle_form_post},
    { "POST", "/api/echo",  handle_echo     },
    { "GET",  "/api/time",  handle_time     },
    { "GET",  "/health",    handle_health   },
    { NULL,   NULL,         NULL            }
};


/* Strip the body for HEAD responses, keeping Content-Length intact. */
static void strip_body_for_head(http_response_t* res)
{
    if (!res) return;
    free(res->body);
    res->body = NULL;
    /* body_size is intentionally preserved; it drives Content-Length. */
}


/* Returns the set of methods (comma-joined) that match this path,
   in a static buffer suitable for the Allow header on a 405. */
static const char* allowed_methods_for(const char* path)
{
    static char buf[64];
    buf[0] = '\0';
    for (size_t i = 0; ROUTES[i].method; i++) {
        if (strcmp(ROUTES[i].path, path) == 0) {
            if (buf[0]) strncat(buf, ", ", sizeof(buf) - strlen(buf) - 1);
            strncat(buf, ROUTES[i].method, sizeof(buf) - strlen(buf) - 1);
        }
    }
    if (buf[0]) {
        /* HEAD piggybacks on every GET. */
        if (strstr(buf, "GET") && !strstr(buf, "HEAD"))
            strncat(buf, ", HEAD", sizeof(buf) - strlen(buf) - 1);
        if (!strstr(buf, "OPTIONS"))
            strncat(buf, ", OPTIONS", sizeof(buf) - strlen(buf) - 1);
    }
    return buf[0] ? buf : NULL;
}


success_flag_t router_dispatch(http_request_t* req, http_response_t* res)
{
    const char* method = req->req_line->method;
    const char* path   = req->req_line->uri;

    /* Strip query string before matching. */
    char path_buf[URI_LENGTH];
    strncpy(path_buf, path, sizeof(path_buf) - 1);
    path_buf[sizeof(path_buf) - 1] = '\0';
    char* q = strchr(path_buf, '?');
    if (q) *q = '\0';

    /* Universal OPTIONS handler. */
    if (strcmp(method, "OPTIONS") == 0) return handle_options(req, res);

    int is_head = (strcmp(method, "HEAD") == 0);
    const char* effective = is_head ? "GET" : method;

    for (size_t i = 0; ROUTES[i].method; i++) {
        if (strcmp(ROUTES[i].path, path_buf) != 0) continue;
        if (strcmp(ROUTES[i].method, effective) != 0) continue;

        success_flag_t rc = ROUTES[i].handler(req, res);
        if (rc == SUCCESS && is_head) strip_body_for_head(res);
        return rc;
    }

    /* Path matched but method didn't → 405 with Allow. */
    const char* allow = allowed_methods_for(path_buf);
    if (allow) return make_405(res, allow);

    /* Static-file fallback. */
    if (strncmp(path_buf, "/static/", 8) == 0 ||
        strncmp(path_buf, "/assets/", 8) == 0)
    {
        success_flag_t rc = handle_static(req, res);
        if (rc == SUCCESS && is_head) strip_body_for_head(res);
        return rc;
    }

    return make_404(res);
}
