#include "../include/handler.h"
success_flag_t make_404_response(http_response_t* res)
{
    res->res_line->status = 404;
    strcpy(res->res_line->reason, "not found");
    res->head = NULL;
    res->body = NULL;
    
    return FAILURE;
}
success_flag_t index_handler(http_request_t* req, http_response_t* res)
{


    long file_size;
    char* content = malloc(RECV_BUFF_SIZE);
    if(!content)
    {
        return make_404_response(res);
    }
    content = load_file("static/index.html", &file_size);

    if (content) {
        res->res_line->status = 200;
        strcpy(res->res_line->reason, "OK");
        strcpy(res->res_line->http_version, "HTTP/1.0");
        res->head->headers[0] = (header_t){
            .name = "Content-type",
            .value = "text/html"
        };
        res->head->headers_counter = 1;
        res->body = malloc(RECV_BUFF_SIZE);
        if(!res->body)
        {
            return make_404_response(res);
        }
        strcpy(res->body, content);
        res->body_size = (size_t) file_size;
        free(content);
        return SUCCESS;
    }

    

    return make_404_response(res);
}