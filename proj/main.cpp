#include <iostream>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include "unicast.h"
#include "exception.h"

void * thread_1(void * arg);
void * thread_2(void * arg);

int main(int argc, char* argv[]) {
    /**
     * call with a.out selfport, id, remote ip 1, remote port 1, remote ip 2, remote port 2
     */
    pthread_t tid1;
    pthread_t tid2;
    Unicast unicast(atoi(argv[1]), 1000);

    pthread_create(&tid1, NULL, thread_1, &unicast);
    pthread_create(&tid2, NULL, thread_2, &unicast);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
}

void * thread_1(void * arg) {
    Unicast * unicast = (Unicast *) arg;
    while(1) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        try {
            std::cout << "thread 1:" << unicast->deliever("for1", 2000) << std::endl;
        } catch(m_exception e) {
            std::cout << "Timeout" << std::endl;
        }
    }
}

void * thread_2(void * arg) {
    Unicast * unicast = (Unicast *) arg;
    while(1) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        try {
            std::cout << "thread 2:" << unicast->deliever("for2", 2000) << std::endl;
        } catch(m_exception e) {
            std::cout << "Timeout" << std::endl;
        }
    }
}
