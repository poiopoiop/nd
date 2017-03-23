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

#include <event2/listener.h>

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

void http_handler(struct evhttp_request *req, void *arg)
{
    int ret_errno = ERRNO_SUCCESS;
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        puts("failed to create response buffer \n");
        return;
    }

    evbuffer_add_printf(buf, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);

    log_notice("errno: %u", ret_errno);
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





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
