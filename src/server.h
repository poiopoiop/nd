/**
 * @file server.h
 * @author cao_yu
 * @date 2017/03/23 10:39:25
 * @brief 
 *  
 **/

#ifndef  __SERVER_H_
#define  __SERVER_H_

#include <sys/socket.h>
#include "event.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"

#include "ndsim_define.h"

typedef struct _req_t {
    unsigned int log_id; //mirror from conn
    char *writebuf;
    event_timer_t *timeout_event;
    bool timeout;
    unsigned long req_id;
    struct timeval start_time;
    int err_no;
    char err_msg[MAX_ERR_MSG_LEN];

    struct evhttp_request *evhttp_req;
} req_t;


int read_conf();

int server_init();
void server_destroy();
int server_run();

void http_handler(struct evhttp_request *req, void *arg);
void timeout_handler(void* arg);

void failure_process(struct evhttp_request *req, struct evbuffer *response_buffer, int ret_errno, const char* ret_errmsg, int ret_status, int cmdno);

int get_post_data(struct evhttp_request *req, char* post_data);

#endif  //__SERVER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
