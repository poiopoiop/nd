/**
 * @file function.cpp
 * @author cao_yu
 * @date 2017/03/23 10:39:25
 * @brief 
 *  
 **/

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>

#include "function.h"
#include "http_client.h"
#include "ndsim_define.h"
#include "log.h"

//int search(struct evhttp_request *req, struct evbuffer *response_buffer, int post_len, const char *post_data) {
int search(req_t * r) {
    struct event_base* base = r->base;

    /*
    struct http_request_post *http_req_post = 
        (struct http_request_post*)start_http_request(base,  
        "http://www.baidu.com/",  
        REQUEST_POST_FLAG,  
        HTTP_CONTENT_TYPE_URL_ENCODED,  
        "name=winlin&code=1234", 0);  
    */
    struct http_request_get *http_req_get1 = 
        (struct http_request_get*)start_http_request(base,  
        "http://www.google.com/",  
        REQUEST_GET_FLAG,  
        NULL, NULL, 1);  
    /*
    struct http_request_get *http_req_get2 = 
        (struct http_request_get*)start_http_request(base,  
        "http://www.baidu.com",  
        REQUEST_GET_FLAG,  
        NULL, NULL, 2);  
    struct http_request_get *http_req_get3 = 
        (struct http_request_get*)start_http_request(base,  
        "http://www.baidu.com",  
        REQUEST_GET_FLAG,  
        NULL, NULL, 3);  
        */
      
    event_base_dispatch(base);  
      
    //http_request_free((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);  
    http_request_free(http_req_get1, REQUEST_GET_FLAG);  
    /*
    http_request_free(http_req_get2, REQUEST_GET_FLAG);  
    http_request_free(http_req_get3, REQUEST_GET_FLAG);  
    */
    event_base_free(base);  

    //write response message
    evbuffer_add_printf(r->writebuf, "Server process done, function: search, errno: %d, errmsg:%s\n", ERRNO_SUCCESS, "success");

    //send response
    evhttp_send_reply(r->evhttp_req, HTTP_OK, "success", r->writebuf);

    log_debug("evhttp_send_reply, %d, %d", r->evhttp_req, r->writebuf);

    return 0;
}

int search2(struct evhttp_request *req, struct evbuffer *response_buffer, int post_len, const char *post_data) {
    struct event_base* base = event_base_new();  

    struct http_request_get *http_req_get1 = 
        (struct http_request_get*)start_http_request(base,  
        "http://www.baidu.com/",  
        REQUEST_GET_FLAG,  
        NULL, NULL, 1);  
     
    event_base_dispatch(base);  

    http_request_free(http_req_get1, REQUEST_GET_FLAG);  
    event_base_free(base);  

    //write response message
    evbuffer_add_printf(response_buffer, "Server process done, function: search2, errno: %d, errmsg:%s\n", ERRNO_SUCCESS, "success");

    //send response
    evhttp_send_reply(req, HTTP_OK, "success", response_buffer);

    log_debug("evhttp_send_reply, %d, %d", req, response_buffer);

    return 0;
}
