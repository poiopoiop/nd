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
//#include "log.h"

std::string conf_dir = "";
std::string conf_name = "";
comcfg::Configure *conf;


int main(int argc, char **argv) {

    if (argc == 1) {
        conf_dir = "./conf";
        conf_name = "proxy.conf";
    } else if (argc == 2) {
        conf_dir = dirname(argv[1]);
        conf_name = basename(argv[1]);
    }

    printf("%s\n%s\n", conf_dir.c_str(), conf_name.c_str());
 
    printf ("hello world\n");
    return 0;
}
