/**
 * @file http_client.cpp
 * @author cao_yu
 * @date 2017/03/23 23:04:49
 * @brief
 *
 **/

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>

#include "http_client.h"
#include "log.h"

void http_request_post_cb(struct evhttp_request *req, void *arg)
{
    struct http_request_post *http_req_post = (struct http_request_post *)arg;
    log_debug("http_request_post_cb, id: %d, status: %d", http_req_post->id, req->response_code);
    switch(req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            //print_request_head_info(req->output_headers);

            char *tmp = (char *)malloc(len+1);
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            free(tmp);

            event_base_loopexit(http_req_post->base, 0);
            break;
        }
        case HTTP_MOVEPERM:
            break;
        case HTTP_MOVETEMP:
        {
            const char *new_location = evhttp_find_header(req->input_headers, 
                    "Location");
            struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);
            evhttp_uri_free(http_req_post->uri);
            http_req_post->uri = new_uri;
            start_url_request((struct http_request_get *)http_req_post, 
                    REQUEST_POST_FLAG);
            return;
        }

        default:
            event_base_loopexit(http_req_post->base, 0);
            return;
    }
}

void http_request_get_cb(struct evhttp_request *req, void *arg)
{
    struct http_request_get *http_req_get = (struct http_request_get *)arg;
    log_debug("http_request_get_cb, id: %d, status: %d", http_req_get->id, req->response_code);
    switch(req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            //print_request_head_info(req->output_headers);

            char *tmp = (char *)malloc(len+1);
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            free(tmp);

            event_base_loopexit(http_req_get->base, 0);
            break;
        }
        case HTTP_MOVEPERM:
            break;
        case HTTP_MOVETEMP:
        {
            const char *new_location = 
                evhttp_find_header(req->input_headers, "Location");
            struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);
            evhttp_uri_free(http_req_get->uri);
            http_req_get->uri = new_uri;
            start_url_request(http_req_get, REQUEST_GET_FLAG);
            return;
        }

        default:
            log_debug("http_request_get_cb, in default");
            event_base_loopexit(http_req_get->base, 0);
            return;
    }
}

int start_url_request(struct http_request_get *http_req, int req_get_flag)
{
    //1. free(evhttp_connection *cn)
    if (http_req->cn)
        evhttp_connection_free(http_req->cn);

    //2. new(evhttp_connection *cn)
    //
    int port = evhttp_uri_get_port(http_req->uri);
    http_req->cn = evhttp_connection_base_new
        (http_req->base,
        NULL,
        evhttp_uri_get_host(http_req->uri),
        (port == -1 ? 80 : port));

    //3. set evhttp_connection timeout
    evhttp_connection_set_timeout(http_req->cn, 6);

    /**
     * Request will be released by evhttp connection
     * See info of evhttp_make_request()
     */
    //4. new(evhttp_request *req)
    //set callback
    if (req_get_flag == REQUEST_POST_FLAG) {
        http_req->req = evhttp_request_new(http_request_post_cb, http_req);
    } else if (req_get_flag ==  REQUEST_GET_FLAG) {
        http_req->req = evhttp_request_new(http_request_get_cb, http_req);
    }

    if (req_get_flag == REQUEST_POST_FLAG) {
        const char *path = evhttp_uri_get_path(http_req->uri);
        evhttp_make_request(http_req->cn, http_req->req, EVHTTP_REQ_POST,
                            path ? path : "/");
        /** Set the post data */
        struct http_request_post *http_req_post = 
            (struct http_request_post *)http_req;
        evbuffer_add(http_req_post->req->output_buffer, 
                http_req_post->post_data, strlen(http_req_post->post_data));
        evhttp_add_header(http_req_post->req->output_headers, 
                "Content-Type", http_req_post->content_type);
    } else if (req_get_flag == REQUEST_GET_FLAG) {
        const char *query = evhttp_uri_get_query(http_req->uri);
        const char *path = evhttp_uri_get_path(http_req->uri);
        size_t len = (query ? strlen(query) : 0) + (path ? strlen(path) : 0) + 1;
        char *path_query = NULL;
        if (len > 1) {
            path_query = (char *)calloc(len, sizeof(char));
            sprintf(path_query, "%s?%s", path, query);
        }
        //5. make request
        evhttp_make_request(http_req->cn, http_req->req, EVHTTP_REQ_GET,
                             path_query ? path_query: "/");
    }
    /** Set the header properties */
    evhttp_add_header(http_req->req->output_headers, 
            "Host", evhttp_uri_get_host(http_req->uri));

    return 0;
}

void print_request_head_info(struct evkeyvalq *header)  
{  
    struct evkeyval *first_node = header->tqh_first;  
    while (first_node) {  
        log_debug("key:%s  value:%s", first_node->key, first_node->value);  
        first_node = first_node->next.tqe_next;  
    }  
}  
  
void print_uri_parts_info(const struct evhttp_uri * http_uri)  
{  
    log_debug("scheme:%s", evhttp_uri_get_scheme(http_uri));  
    log_debug("host:%s", evhttp_uri_get_host(http_uri));  
    log_debug("path:%s", evhttp_uri_get_path(http_uri));  
    log_debug("port:%d", evhttp_uri_get_port(http_uri));  
    log_debug("query:%s", evhttp_uri_get_query(http_uri));  
    log_debug("userinfo:%s", evhttp_uri_get_userinfo(http_uri));  
    log_debug("fragment:%s", evhttp_uri_get_fragment(http_uri));  
}  

void *http_request_new(struct event_base* base, const char *url, 
        int req_get_flag, const char *content_type, 
        const char* data, int id)
{
    int len = 0;
    if (req_get_flag == REQUEST_GET_FLAG) {
        len = sizeof(struct http_request_get);
    } else if(req_get_flag == REQUEST_POST_FLAG) {
        len = sizeof(struct http_request_post);
    }

    struct http_request_get *http_req_get = 
        (struct http_request_get*)calloc(1, len);
    http_req_get->uri = evhttp_uri_parse(url);
    //print_uri_parts_info(http_req_get->uri);

    http_req_get->base = base;
    http_req_get->id   = id;

    if (req_get_flag == REQUEST_POST_FLAG) {
        struct http_request_post *http_req_post = 
            (struct http_request_post *)http_req_get;
        if (content_type == NULL) {
            content_type = HTTP_CONTENT_TYPE_URL_ENCODED;
        }
        http_req_post->content_type = strdup(content_type);

        if (data == NULL) {
            http_req_post->post_data = NULL;
        } else {
            http_req_post->post_data = strdup(data);
        }
    }

    return http_req_get;
}

void http_request_free(struct http_request_get *http_req_get, int req_get_flag)
{
    evhttp_connection_free(http_req_get->cn);
    evhttp_uri_free(http_req_get->uri);
    if (req_get_flag == REQUEST_GET_FLAG) {
        free(http_req_get);
    } else if(req_get_flag == REQUEST_POST_FLAG) {
        struct http_request_post *http_req_post = 
            (struct http_request_post*)http_req_get;
        if (http_req_post->content_type) {
            free(http_req_post->content_type);
        }
        if (http_req_post->post_data) {
            free(http_req_post->post_data);
        }
        free(http_req_post);
    }
    http_req_get = NULL;
}

void *start_http_request(struct event_base* base,
        const char *url, int req_get_flag,
        const char *content_type, const char* data,
        int id) {

    struct http_request_get *http_req_get =
        (struct http_request_get *)http_request_new
            (base, url, req_get_flag, content_type, data, id);
    start_url_request(http_req_get, req_get_flag);

    return http_req_get;
}


