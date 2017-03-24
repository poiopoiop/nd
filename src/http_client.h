/**
 * @file http_client.h
 * @author cao_yu
 * @date 2017/03/23 21:17:49
 * @brief 
 *  
 **/

#ifndef  __HTTP_CLIENT_H_
#define  __HTTP_CLIENT_H_


// (default)  
#define HTTP_CONTENT_TYPE_URL_ENCODED   "application/x-www-form-urlencoded"     
// (use for files: picture, mp3, tar-file etc.)                                          
#define HTTP_CONTENT_TYPE_FORM_DATA     "multipart/form-data"                   
// (use for plain text)  
#define HTTP_CONTENT_TYPE_TEXT_PLAIN    "text/plain"  

#define REQUEST_POST_FLAG               2  
#define REQUEST_GET_FLAG                3  

struct http_request_get {  
    struct evhttp_uri *uri;  
    struct event_base *base;  
    struct evhttp_connection *cn;  
    struct evhttp_request *req;  
    int id;
};  
  
struct http_request_post {  
    struct evhttp_uri *uri;  
    struct event_base *base;  
    struct evhttp_connection *cn;  
    struct evhttp_request *req;  
    char *content_type;  
    char *post_data;  
    int id;
};  
 
//request function
void http_request_post_cb(struct evhttp_request *req, void *arg);  
void http_request_get_cb(struct evhttp_request *req, void *arg);  
int start_url_request(struct http_request_get *http_req, int req_get_flag);  

//tools function
void print_request_head_info(struct evkeyvalq *header);
void print_uri_parts_info(const struct evhttp_uri * http_uri);

//new/free function 
void *http_request_new(struct event_base* base, const char *url, int req_get_flag,
                       const char *content_type, const char* data, int id);
void http_request_free(struct http_request_get *http_req_get, int req_get_flag);

//start post/get function
void *start_http_request(struct event_base* base, const char *url, 
        int req_get_flag, const char *content_type, const char* data, 
        int id);

 
#endif  //__HTTP_CLIENT_H_
