#ifndef CHORD_H
#define CHORD_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <thread>
#include <pthread.h>
#include <regex>
#include <tuple>

#include "unicast.h"

#define MAX_NUM_PEERS 256
#define NUM_SUCCESSORS 2
#define CHORD_TAG "chord"
#define CHORD_REGEX "<[^(<|>)]+>*"

class Chord {
public:
    Chord(int pid) : self_id(pid), 
                     running(false), expression(CHORD_REGEX),
                     set_cond(PTHREAD_COND_INITIALIZER),
                     get_cond(PTHREAD_COND_INITIALIZER),
                     mutex(PTHREAD_MUTEX_INITIALIZER){
        self_hash = hash((unsigned char *) &pid, 4);
    };
    ~Chord();
    void set_peers(std::map<int, std::pair<std::string, int> > & table);
    int set(std::string key, std::string value);
    std::string get(std::string key);
    std::vector<int> owner(std::string key);
    std::string list_local();
    const Unicast & get_unicast();

    void begin();
    void end();

private:
    int self_id;
    std::pair<std::string, int> self_addr;
    unsigned char self_hash;
    Unicast cast_helper;
    std::thread background_thrd;
    mutable pthread_mutex_t mutex;
    mutable pthread_cond_t set_cond;
    mutable pthread_cond_t get_cond;
    const std::regex expression;
    //                    hash val          ipaddr    portnum
    std::vector<std::pair<int, std::pair<std::string, int> > > finger_table;
    //                    id                ipaddr    portnum
    std::vector<std::pair<int, std::pair<std::string, int> > > successors;   // successors in order


    // need protection
    bool running;
    std::map<std::string, std::string> local_table;
    bool get_success;
    std::string get_value;
    std::vector<int> owners;

    // functions
    unsigned char hash(const unsigned char * char_arr, int length);
    void deamon();
};

#endif