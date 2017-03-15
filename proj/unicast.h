#ifndef UNICAST_H
#define UNICAST_H

#include <string>
#include <vector>
#include <map>

class Unicast {
public:

    Unicast (int portnum);

    Unicast (int portnum, int max_delay);

    void send (std::string msg, std::string ipaddr, int port);

    void send (std::string tag, std::string msg, std::string ipaddr, int port);

    int get_port();

    bool running();

    void stop();

    std::string deliever ();

    std::string deliever (std::string tag);

    void message_arrives(std::string msg);

    int get_delay_bound();

private:
    int port;

    std::vector<std::string> sendbuf;
    std::string rec_msg;

    int delay_bound;        // delay in ms

    pthread_t server_thrd;
    std::vector<pthread_t> sender_thrds;

    volatile bool terminated;

    pthread_cond_t wait;
    std::map<std::string, pthread_cond_t> wait_conds;
    pthread_mutex_t mutex;
};

#endif