/**
 * @file ndsim_define.h
 * @author cao_yu
 * @date 2017/03/23 15:22:21
 * @brief 
 *  
 **/

#ifndef  __NDSIM_DEFINE_H_
#define  __NDSIM_DEFINE_H_

//errno
#define ERRNO_SUCCESS               0
#define ERRNO_SYSTEM_ERROR          -1
#define ERRNO_MEM_PROBLEM           -2
#define ERRNO_POST_DATA_TOO_LARGE   -10


//for multi-thread, default thread num
#define THREAD_NUM_DEFAULT          4


//for request
//max input post data size if 16M, for pure txt
#define MAX_POST_DATA_SIZE          16000000

#endif  //__NDSIM_DEFINE_H_
