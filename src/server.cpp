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

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>

#include <Configure.h>

#include "ndsim_define.h"
#include "server.h"
#include "event.h"
#include "log.h"
#include "http_client.h"
#include "function.h"

extern comcfg::Configure *conf;
int timeout_s;
static int port;

//for multiple thread
int thread_num;

//global variable
char *post_data;

int read_conf() {
    try {
        port = conf[0]["Server"]["Port"].to_int32();
        conf[0]["Server"]["Thread_Num"].get_int32(&thread_num, THREAD_NUM_DEFAULT);
        if (thread_num <= 0) {
            thread_num = THREAD_NUM_DEFAULT;
        }
        log_trace("thread_num conf load done, thread_num: %u", thread_num);

        timeout_s = conf[0]["Server"]["Timeout_s"].to_int32();
        if (timeout_s <= 0) {
            timeout_s = TIMEOUT_S_DEFAULT;
        }
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

    //post_data init, ready to parse and store post data
    post_data = (char *) malloc(MAX_POST_DATA_SIZE + 1);
    if (!post_data) {
        log_fatal("failed to create post_data buffer \n");
        exit(-1);
    }

    server_run();
    return 0;
}

void server_destroy() {
    //free post_data
    free(post_data);
    return;
}

int server_run() {
    struct event_base * server_base = event_base_new();
    struct evhttp * http_server = evhttp_new(server_base);
    if(!http_server)
    {
        return -1;
    }

    int ret = evhttp_bind_socket(http_server, "127.0.0.1", port);
    if(ret!=0)
    {
        return -1;
    }

    //add timeout, timeout in sec
    //of no use, would not halt request, but only have empty reply after timeout
    //evhttp_set_timeout(http_server, 3);

    evhttp_set_gencb(http_server, http_handler, server_base);

    log_trace("http server start OK!");

    event_base_dispatch(server_base);

    evhttp_free(http_server);

    return 0;
}

int get_post_data(struct evhttp_request *req, char* post_data) {
    //input_buffer, get POST info
    evbuffer * input_buffer = evhttp_request_get_input_buffer(req);

    //length of post data
    int len = evbuffer_get_length(input_buffer);
    if (len <= 0) {
        log_warning("post data empty");
        log_notice("errno: %d", ERRNO_POST_DATA_EMPTY);
        return ERRNO_POST_DATA_EMPTY;
    }
    else if (len > MAX_POST_DATA_SIZE) {
        log_warning("post data too large: %u > %u", len, MAX_POST_DATA_SIZE);
        log_notice("errno: %d", ERRNO_POST_DATA_TOO_LARGE);
        return ERRNO_POST_DATA_TOO_LARGE;
    }

    memcpy(post_data, evbuffer_pullup(input_buffer, -1), len);

    return len;
}

void connection_handler(struct evhttp_connection *evcon, void *arg) {
    log_debug("in connection_handle");
    return;
}


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
