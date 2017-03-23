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

void failure_process(struct evhttp_request *req, struct evbuffer *response_buffer, int ret_errno, const char* ret_errmsg, int ret_status, int cmdno=CMDNO_UNKNOWN) {
    //write response message
    evbuffer_add_printf(response_buffer, "Server process failed, errno: %d, errmsg:%s\n", ret_errno, ret_errmsg);

    //send response
    evhttp_send_reply(req, ret_status, ret_errmsg, response_buffer);

    //log
    log_warning(ret_errmsg);

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

void http_handler(struct evhttp_request *req, void *arg)
{
    struct timeval start_time, end_time;
    event_gettime(&start_time);

    int ret_errno   = 0;
    int post_len    = 0;
    int command_no  = 0;

    sleep(1);
    //memory init
    //build response_buffer
    struct evbuffer *response_buffer = evbuffer_new();
    if(!response_buffer) {
        log_fatal("failed to create response buffer \n");
        exit(-1);
    }
    //post_data init, ready to parse and store post data
    char *post_data = (char *) malloc(MAX_POST_DATA_SIZE + 1);
    if (!post_data) {
        //free response buffer
        evbuffer_free(response_buffer);
        log_fatal("failed to create post_data buffer \n");
        exit(-1);
    }
    memset(post_data, '\0', MAX_POST_DATA_SIZE + 1);

    //parse get parameters 
    struct evkeyvalq * get_params;
    get_params = evhttp_request_get_input_headers(req);
    evhttp_parse_query(evhttp_request_get_uri(req), get_params);
    //get cmdno from request
    const char *cmdno = evhttp_find_header(get_params, "cmdno");
    if (cmdno == 0 || *cmdno == '\0') {
        command_no = 0;
    }
    else {
        command_no = atoi(cmdno);
    }

    //command distribution
    switch(command_no) {
        case CMDNO_SEARCH:
            post_len = get_post_data(req, post_data);
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_SAMPLE_AND_SIGN:
            post_len = get_post_data(req, post_data);
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_GET_DOCS_BY_SIGN:
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_GET_SIGNS_BY_DOC:
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_GET_DOCS_BY_DOC:
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_WORDSEG:
            post_len = get_post_data(req, post_data);
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        case CMDNO_CLASS:
            post_len = get_post_data(req, post_data);
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_NOT_SUPPORT_CMDNO, "cmdno not support", HTTP_NOTFOUND);
            break;
        default:
            ret_errno = ERRNO_ILLEGAL_CMDNO;
            failure_process(req, response_buffer, ERRNO_ILLEGAL_CMDNO, "illegal cmdno", HTTP_BADREQUEST);
            break;
    }

    /*
    //write response message
    evbuffer_add_printf(response_buffer, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));

    //send response
    evhttp_send_reply(req, HTTP_OK, "OK", response_buffer);
    */

    //free response buffer
    evbuffer_free(response_buffer);

    //free post_data
    free(post_data);

    event_gettime(&end_time);
    float cost = ((end_time.tv_sec-start_time.tv_sec)*1000+(end_time.tv_usec-start_time.tv_usec)/1000.0);
    log_notice("errno: %d, cmdno: %d, cost: %.0f, len: %u", ret_errno, command_no, cost, post_len);

    return;
}





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
