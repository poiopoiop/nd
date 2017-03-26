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
#define ERRNO_ILLEGAL_CMDNO         -10
#define ERRNO_NOT_SUPPORT_CMDNO     -11
#define ERRNO_POST_DATA_TOO_LARGE   -12
#define ERRNO_POST_DATA_EMPTY       -13


//for multi-thread, default thread num
#define THREAD_NUM_DEFAULT          4


//for request
//max input post data size if 16M, for pure txt
#define MAX_POST_DATA_SIZE          16000000
#define MAX_ERR_MSG_LEN             128


//for command/interface
#define CMDNO_UNKNOWN               0
#define CMDNO_SEARCH                1
#define CMDNO_SAMPLE_AND_SIGN       2
#define CMDNO_GET_DOCS_BY_SIGN      3
#define CMDNO_GET_SIGNS_BY_DOC      4
#define CMDNO_GET_DOCS_BY_DOC       5
#define CMDNO_WORDSEG               6
#define CMDNO_CLASS                 7

#endif  //__NDSIM_DEFINE_H_
