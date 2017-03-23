/**
 * @file main.cpp
 * @author cao_yu
 * @date 2017/03/22 16:12:21
 * @brief 
 *  
 **/
#include <Configure.h>
#include <stdio.h>
#include <signal.h>

#include "file.h"
#include "event.h"
#include "server.h"
#include "log.h"

std::string conf_dir = "";
std::string conf_name = "";
comcfg::Configure *conf; 
#define ENSURE(x, msg) do { if(x) { log_fatal(msg); return 1; } } while(0)

int signal_handler_init() {
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGPIPE, &sa, NULL))
        return -1;
    return 0;
}

#ifndef UNIT_TEST
int main(int argc, char **argv) {

    if (argc == 1) {
        conf_dir = "./conf";
        conf_name = "ndsim.conf";
    } else if (argc == 2) {
        conf_dir = dirname(argv[1]);
        conf_name = basename(argv[1]);
    } else if (argc == 3) {
        conf_dir = argv[1];
        conf_name = argv[2];
    } else {
        fprintf(stderr, "usage: %s path_to_conf_file\n", argv[0]);
        return 1;
    }

    if (signal_handler_init()) {
        fprintf(stderr, "Initialize signal handler failed!\n");
        return 1;
    }
    conf = new comcfg::Configure();
    if (conf->load(conf_dir.c_str(), conf_name.c_str())) {
        fprintf(stderr, "Load conf fail. dir:%s, name:%s\n", conf_dir.c_str(), conf_name.c_str());
        return 1;
    }
    //mkdir log before start this server
    if (log_init() != 0) {
        fprintf(stderr, "log init failed\n");
        return 1;
    }
    log_trace("log init done");

    ENSURE(event_init(), "event init failed");
    log_trace("event init done");

    ENSURE(server_init(), "event init failed");
    log_trace("server init done");
 

    printf("%s\n%s\n", conf_dir.c_str(), conf_name.c_str());
 
    printf ("hello world\n");
    return 0;
}
#endif
