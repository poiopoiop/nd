/**
 * @file ndsim_define.h
 * @author cao_yu
 * @date 2017/03/23 15:22:21
 * @brief 
 *  
 **/

#ifndef  __NDSIM_DEFINE_H_
#define  __NDSIM_DEFINE_H_

#include "event.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"

//errno
#define ERRNO_SUCCESS               0
#define ERRNO_SYSTEM_ERROR          -1
#define ERRNO_MEM_PROBLEM           -2
#define ERRNO_ILLEGAL_CMDNO         -10
#define ERRNO_NOT_SUPPORT_CMDNO     -11
#define ERRNO_POST_DATA_TOO_LARGE   -12
#define ERRNO_POST_DATA_EMPTY       -13


//for multi-thread, default thread num
#define THREAD_NUM_DEFAULT          4

#define TIMEOUT_S_DEFAULT           100

//for request
//max input post data size if 16M, for pure txt
#define MAX_POST_DATA_SIZE          16000000
#define MAX_ERR_MSG_LEN             128
#define MAX_CLIENT_BASE_NUM         10000


//for command/interface
#define CMDNO_UNKNOWN               0
#define CMDNO_SEARCH                1
#define CMDNO_SAMPLE_AND_SIGN       2
#define CMDNO_GET_DOCS_BY_SIGN      3
#define CMDNO_GET_SIGNS_BY_DOC      4
#define CMDNO_GET_DOCS_BY_DOC       5
#define CMDNO_WORDSEG               6
#define CMDNO_CLASS                 7
#define CMDNO_TEST_BACKEND          12

typedef struct _client_base_t {
    struct event_base * client_base;
    struct event_base * last_base;
    struct event_base * next_base;
}client_base_t;

typedef struct _req_t {

    unsigned int log_id; //mirror from conn
    char *readbuf;
    int readbuf_len;

    //char *writebuf;
    struct evbuffer * writebuf;

    event_timer_t *timeout_event;
    bool timeout;
    unsigned long req_id;
    struct timeval start_time;

    int command_no;
    int err_no;
    char err_msg[MAX_ERR_MSG_LEN];

    struct evhttp_request *evhttp_req;

    //request base, only 1 for each request
    struct event_base * req_base;

    //client bases, every backend request has a single base
    client_base_t * client_base_head[MAX_CLIENT_BASE_NUM];
    int client_num;

} req_t;


#endif  //__NDSIM_DEFINE_H_
