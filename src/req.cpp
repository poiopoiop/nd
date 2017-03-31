/**
 * @file req.cpp
 * @author cao_yu
 * @date 2017/03/27 10:39:25
 * @brief 
 *  
 **/

#include "req.h"

#include <string.h>
#include <errno.h>

#include <netinet/in.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>

#include <Configure.h>
#include "log.h"
#include "server.h"
#include "http_client.h"
#include "function.h"

extern int timeout_s;

//global variable
char *post_data;

int req_init() {
    //post_data init, ready to parse and store post data
    post_data = (char *) malloc(MAX_POST_DATA_SIZE + 1);
    if (!post_data) {
        log_fatal("failed to create post_data buffer \n");
        return -1;
    }

    return 0;
}

req_t * req_new() {
    req_t *r = (req_t*)malloc(sizeof(req_t));
    r->readbuf = post_data;
    struct event_base* rbase = event_base_new();  
    r->req_base = rbase;
    r->timeout_event = event_timer_new(timeout_handler, timeout_s * 1000000, false, r, r->req_base);
    //r->timeout_event = event_timer_new(req_timeout, timeout.tv_sec * 1000000 + timeout.tv_usec, false, r);

    r->readbuf_len  = 0;
    r->err_no       = 0;
    r->command_no   = 0;

    //memory init
    //build response_buffer
    r->writebuf = evbuffer_new();
    if(!r->writebuf) {
        log_fatal("failed to create response buffer \n");
        exit(-1);
    }

    return r;
}

void req_free(req_t * r) {
    //free response buffer
    evbuffer_free(r->writebuf);
    return;
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

void timeout_handler(void* arg) {
    req_t *r = (req_t*)arg;
    r->timeout = true;
    printf("!!! in timeout_handler !!!\n");

    //write response message
    evbuffer_add_printf(r->writebuf, "Server Responsed Timeout. Requested: %s\n", evhttp_request_get_uri(r->evhttp_req));

    //send response
    evhttp_send_reply(r->evhttp_req, 504, "Timeout", r->writebuf);

    event_base_loopexit(r->req_base, 0);
    //event_base_free(r->req_base);  

    return;
}

void http_handler(struct evhttp_request *evhttp_req, void *arg) {
    log_debug("in http_handler");
    struct timeval start_time, end_time;
    event_gettime(&start_time);

    req_t * r = req_new();
    r->evhttp_req = evhttp_req;

    //post_data init, ready to parse and store post data
    //seems do not necessary to memset
    //memset(post_data, '\0', MAX_POST_DATA_SIZE + 1);

    //set timeout, of no use...
    //evhttp_connection_set_timeout2(req->evcon, 3);
    //evhttp_connection_set_closecb(req->evcon, connection_handler, NULL);

    //parse get parameters 
    struct evkeyvalq * get_params;
    get_params = evhttp_request_get_input_headers(evhttp_req);
    evhttp_parse_query(evhttp_request_get_uri(evhttp_req), get_params);
    //get cmdno from request
    const char *cmdno = evhttp_find_header(get_params, "cmdno");
    if (cmdno == 0 || *cmdno == '\0') {
        r->command_no = 0;
    }
    else {
        r->command_no = atoi(cmdno);
    }

    //command distribution
    switch(r->command_no) {
        case CMDNO_SEARCH:
            r->readbuf_len = get_post_data(evhttp_req, post_data);
            r->err_no = search(r);
            break;
        case CMDNO_SAMPLE_AND_SIGN:
            r->readbuf_len = get_post_data(evhttp_req, post_data);
            r->err_no = search2(evhttp_req, r->writebuf, r->readbuf_len, (const char*)post_data);
            break;
        case CMDNO_GET_DOCS_BY_SIGN:
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_NOT_SUPPORT_CMDNO, 
                    "cmdno not support", HTTP_NOTFOUND, CMDNO_GET_DOCS_BY_SIGN);
            break;
        case CMDNO_GET_SIGNS_BY_DOC:
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_NOT_SUPPORT_CMDNO, 
                    "cmdno not support", HTTP_NOTFOUND, CMDNO_GET_SIGNS_BY_DOC);
            break;
        case CMDNO_GET_DOCS_BY_DOC:
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_NOT_SUPPORT_CMDNO, 
                    "cmdno not support", HTTP_NOTFOUND, CMDNO_GET_DOCS_BY_DOC);
            break;
        case CMDNO_WORDSEG:
            r->readbuf_len = get_post_data(evhttp_req, post_data);
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_NOT_SUPPORT_CMDNO, 
                    "cmdno not support", HTTP_NOTFOUND, CMDNO_WORDSEG);
            break;
        case CMDNO_CLASS:
            r->readbuf_len = get_post_data(evhttp_req, post_data);
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_NOT_SUPPORT_CMDNO, 
                    "cmdno not support", HTTP_NOTFOUND, CMDNO_CLASS);
            break;
        case CMDNO_TEST_BACKEND:
            r->err_no = test_backend(r);
            break;
        default:
            r->err_no = ERRNO_ILLEGAL_CMDNO;
            failure_process(evhttp_req, r->writebuf, ERRNO_ILLEGAL_CMDNO, "illegal cmdno", HTTP_BADREQUEST, ERRNO_ILLEGAL_CMDNO);
            break;
    }

    /*
    //write response message
    evbuffer_add_printf(response_buffer, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));

    //send response
    evhttp_send_reply(req, HTTP_OK, "OK", response_buffer);
    */

    req_free(r);


    event_gettime(&end_time);
    float cost = ((end_time.tv_sec-start_time.tv_sec)*1000+(end_time.tv_usec-start_time.tv_usec)/1000.0);
    log_notice("errno: %d, cmdno: %d, cost: %.3f, len: %d", r->err_no, r->command_no, cost, r->readbuf_len);

    return;
}

void failure_process(struct evhttp_request *req, struct evbuffer *response_buffer, int ret_errno, const char* ret_errmsg, int ret_status, int cmdno=CMDNO_UNKNOWN) {
    //write response message
    evbuffer_add_printf(response_buffer, "Server process failed, errno: %d, errmsg:%s\n", ret_errno, ret_errmsg);

    //send response
    evhttp_send_reply(req, ret_status, ret_errmsg, response_buffer);

    //log
    log_warning(ret_errmsg);

    return;
}





