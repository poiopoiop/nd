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
#include "req.h"

int read_conf();

int server_init();
void server_destroy();
int server_run();

#endif  //__SERVER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
