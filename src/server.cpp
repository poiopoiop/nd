/**
 * @file server.cpp
 * @author cao_yu
 * @date 2017/03/23 10:40:24
 * @brief 
 *  
 **/

#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"
//#include "evhttp.h"

#include <Configure.h>

#include "ndsim_define.h"
#include "server.h"
#include "log.h"

extern comcfg::Configure *conf;
static int port;

//for multiple thread
int thread_num;

int read_conf() {
    try {
        port = conf[0]["Server"]["Port"].to_int32();
        conf[0]["Server"]["Thread_Num"].get_int32(&thread_num, THREAD_NUM_DEFAULT);
        if (thread_num <= 0) {
            thread_num = THREAD_NUM_DEFAULT;
        }
        log_trace("thread_num conf load done, thread_num: %u", thread_num);
    } catch (comcfg::ConfigException &e) {
        log_fatal("Read conf error during server init, errmsg: %s", e.what());
        return -1;
    }
    return 0;
}


int server_init() {
    if (read_conf()) {
        return -1;
    }

    server_run();
    return 0;
}

int server_run() {
    struct event_base * base = event_base_new();
    struct evhttp * http_server = evhttp_new(base);
    if(!http_server)
    {
        return -1;
    }

    int ret = evhttp_bind_socket(http_server, "127.0.0.1", port);
    if(ret!=0)
    {
        return -1;
    }

    evhttp_set_gencb(http_server, http_handler, NULL);

    log_trace("http server start OK!");

    event_base_dispatch(base);

    evhttp_free(http_server);

    return 0;
}

void http_handler(struct evhttp_request *req, void *arg)
{
    //input_buffer, get POST info
    evbuffer * input_buffer = evhttp_request_get_input_buffer(req);
    int buffer_data_len = evbuffer_get_length(input_buffer);
    if (buffer_data_len > MAX_POST_DATA_SIZE) {
        log_warning("post data too large: %u > %u", buffer_data_len, MAX_POST_DATA_SIZE);
        log_notice("errno: %u", ERRNO_POST_DATA_TOO_LARGE);
        return;
    }

    char *post_data = (char *) malloc(buffer_data_len + 1);
    memset(post_data, '\0', buffer_data_len + 1);
    memcpy(post_data, evbuffer_pullup(input_buffer, -1), buffer_data_len);

    //build response_buffer
    struct evbuffer *response_buffer = evbuffer_new();
    if(!response_buffer)
    {
        log_fatal("failed to create response buffer \n");
        log_notice("errno: %u", ERRNO_MEM_PROBLEM);
        return;
    }

    //write response message
    evbuffer_add_printf(response_buffer, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));

    //send response
    evhttp_send_reply(req, HTTP_OK, "OK", response_buffer);

    //free response buffer
    evbuffer_free(response_buffer);

    log_notice("errno: %u, post_data_size: %u", ERRNO_SUCCESS, buffer_data_len);
    return;
}





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
