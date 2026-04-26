#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "utils.h"

/*
   A handler reads the request and fills the response. It allocates
   res->res_line, res->head, and (optionally) res->body. The caller is
   responsible for freeing those after the response is sent.

   Handlers must NOT set Date / Server / Content-Length / Connection;
   the response builder appends those.

   Returning FAILURE means the handler bailed without producing a valid
   response — the connection loop will synthesise a 500.
*/

success_flag_t handle_index   (http_request_t* req, http_response_t* res);
success_flag_t handle_about   (http_request_t* req, http_response_t* res);
success_flag_t handle_contact (http_request_t* req, http_response_t* res);
success_flag_t handle_form_get(http_request_t* req, http_response_t* res);
success_flag_t handle_form_post(http_request_t* req, http_response_t* res);
success_flag_t handle_echo    (http_request_t* req, http_response_t* res);
success_flag_t handle_time    (http_request_t* req, http_response_t* res);
success_flag_t handle_health  (http_request_t* req, http_response_t* res);
success_flag_t handle_options (http_request_t* req, http_response_t* res);

/* Static-file fallback. Resolves the URI under static/ and refuses any
   path containing "..". */
success_flag_t handle_static  (http_request_t* req, http_response_t* res);

/* Standard error responses. */
success_flag_t make_status_response(http_response_t* res,
                                    int status, const char* reason,
                                    const char* body_text);
success_flag_t make_404(http_response_t* res);
success_flag_t make_405(http_response_t* res, const char* allow);
success_flag_t make_500(http_response_t* res);

#endif
