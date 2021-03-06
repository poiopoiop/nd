/**
 * @file req.h
 * @author cao_yu
 * @date 2017/03/27 10:39:25
 * @brief 
 *  
 **/

#ifndef  __REQ_H_
#define  __REQ_H_

#include <sys/socket.h>
#include "event.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"

#include "ndsim_define.h"

int req_init();

//process request timeout
req_t * req_new();
void req_free(req_t * r);

int get_post_data(struct evhttp_request *req, char* post_data);

void http_handler(struct evhttp_request *req, void *arg);
void timeout_handler(void* arg);
void failure_process(struct evhttp_request *req, struct evbuffer *response_buffer, int ret_errno, const char* ret_errmsg, int ret_status, int cmdno);

#endif  //__REQ_H_
