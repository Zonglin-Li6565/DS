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
     * call with a.out selfport, remote ip 1, remote port 1, id
     */
    Unicast unicast(atoi(argv[1]), 5000);
    while(1) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        unicast.send("hello from" + std::string(argv[4]), argv[2], atoi(argv[3]));
        std::cout << unicast.deliever() << std::endl;
    }
}