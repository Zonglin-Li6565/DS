#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <regex>
#include <time.h>
#include <errno.h>

#include "unicast.h"
#include "exception.h"

#define IN_BUF_SIZE     256

struct connect_info {
    Unicast * unicast;
    int sockfd;
};

void * receiver_thread(void *arg);
void * single_connect_thread(void *arg);
void * sender_thread(void *arg);

Unicast::Unicast (int portnum) : port(portnum), mutex(PTHREAD_MUTEX_INITIALIZER){}

Unicast::Unicast (int portnum, int max_delay) : port(portnum), delay_bound(max_delay), mutex(PTHREAD_MUTEX_INITIALIZER){}

int Unicast::send (std::string tag, std::string msg, std::string host_ip, int host_port) {
    msg = "<" + tag + ">" + msg;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("error to open sender socket\n");
        return -2;
    }
    struct sockaddr_in remoteaddr;
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_addr.s_addr = inet_addr(host_ip.c_str());
    remoteaddr.sin_port = htons(host_port);
    if (connect(sockfd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) < 0) {
        printf("failed to connect server %s : %d\n", host_ip.c_str(), host_port);
        close(sockfd);
        return -1;
    }
    if (write(sockfd, msg.c_str(), msg.size()))
    close(sockfd);
    return 0;
}

std::string Unicast::deliever (std::string tag) {
    return deliever(tag, -1);
}

std::string Unicast::deliever (std::string tag, int timeout_ms) {
    std::string copy;
    pthread_cond_t * cond;
    struct timespec ts;
    pthread_mutex_lock(&mutex);
    if (wait_conds.find(tag) == wait_conds.end()) {
        wait_conds[tag] = PTHREAD_COND_INITIALIZER;
    }
    cond = &wait_conds[tag];

    if (timeout_ms >= 0) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;           // nano second
        int ret = pthread_cond_timedwait(cond, &mutex, &ts);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&mutex);
            throw TIME_OUT;
        }
    } else {
        pthread_cond_wait(cond, &mutex);        // sleep on condition var
    }

    copy = rec_msg;
    pthread_mutex_unlock(&mutex);
    return copy;
}

int Unicast::get_port() {
    return port;
}

bool Unicast::running() {
    bool copy;
    pthread_mutex_lock(&mutex);
    copy = !terminated;
    pthread_mutex_unlock(&mutex);
    return copy;
}

void Unicast::begin() {
    pthread_mutex_lock(&mutex);
    if (!terminated) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_mutex_unlock(&mutex);
    pthread_create(&server_thrd, NULL, receiver_thread, this);
}

void Unicast::stop() {
    pthread_mutex_lock(&mutex);
    terminated = true;
    pthread_mutex_unlock(&mutex);
}

void Unicast::message_arrives(std::string msg) {
    std::smatch match;
    std::regex expression("^<.+>");
    pthread_cond_t * cond;
    std::regex_search (msg, match, expression);
    if (match.empty()) {
        return;
    }
    std::string tag = msg.substr(match.position(0) + 1, match.position(0) + match.length(0) - 2);
    pthread_mutex_lock(&mutex);
    if (wait_conds.find(tag) == wait_conds.end()) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    rec_msg = msg.substr(match.position(0) + match.length(0), msg.size());
    cond = &wait_conds[tag];
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(cond);       // wake up all
}

int Unicast::get_delay_bound() {
    return delay_bound;
}

void * single_connect_thread(void *arg) {
    connect_info * nc = (connect_info *) arg;
    Unicast * uc = nc->unicast;
    std::string msg;
    char buffer[IN_BUF_SIZE];
    int n, sockfd = nc->sockfd;
    do {
        n = read(sockfd, buffer, IN_BUF_SIZE - 1);
        buffer[n] = '\0';
        msg += buffer;
    } while (n > 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % uc->get_delay_bound()));
    uc->message_arrives(msg);
    delete nc;
    close(sockfd);
    return 0;
}

void * receiver_thread(void *arg) {
    Unicast * uc = (Unicast *)arg;
    int sockfd, newsockfd, portno = uc->get_port();
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("ERROR opening socket\n");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        printf("ERROR on binding\n");
    while (uc->running()) {
        pthread_t new_thread;
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            printf("ERROR on accept\n");

        connect_info * newconnect = new connect_info();
        newconnect->unicast = uc;
        newconnect->sockfd = newsockfd;
        pthread_create(&new_thread, NULL, single_connect_thread, (void *)newconnect);
    }
    close(sockfd);
    return 0;
}
