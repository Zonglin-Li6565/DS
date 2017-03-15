#include <iostream>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include "unicast.h"

int main(int argc, char* argv[]) {
    /**
     * call with a.out selfport, id, remote ip 1, remote port 1, remote ip 2, remote port 2
     */
    // Unicast unicast(atoi(argv[1]), 5000);
    // while(1) {
    //     std::this_thread::sleep_for (std::chrono::seconds(1));
    //     unicast.send("hello from" + std::string(argv[2]), argv[3], atoi(argv[4]));
    //     std::cout << unicast.deliever() << std::endl;
    //     unicast.send("hello from" + std::string(argv[2]), argv[5], atoi(argv[6]));
    // }
    Unicast unicast(atoi(argv[1]), 1000);
    while(1) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        unicast.send("test", "hello from" + std::string("test process"), "127.0.0.1", 12281);
        std::cout << unicast.deliever("back") << std::endl;
    }
}