/**
 * @file function.h
 * @author cao_yu
 * @date 2017/03/23 10:39:25
 * @brief 
 *  
 **/

#ifndef  __FUNCTION_H_
#define  __FUNCTION_H_

#include "ndsim_define.h"

//int search(struct evhttp_request *req, struct evbuffer *response_buffer, int post_len, const char *post_data);
int search(req_t * r);
int search2(struct evhttp_request *req, struct evbuffer *response_buffer, int post_len, const char *post_data);

int test_backend(req_t * r);




#endif  //__SERVER_H_
