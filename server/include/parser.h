#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#ifndef PARSER_H
#define PARSER_H

success_flag_t parse_req(char* raw, http_request_t* req);
success_flag_t parse_res(const char* raw, http_response_t* req);


#endif