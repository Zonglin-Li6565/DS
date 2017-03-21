#ifndef CHORD_H
#define CHORD_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <thread>
#include <pthread.h>

#include "unicast.h"

#define MAX_NUM_PEERS 256
#define CHORD_TAG "chord"

class Chord {
public:
    Chord(int pid) : self_id(pid), cast_helper(pid), running(false){
        self_hash = hash((unsigned char *) &pid, 4);
    };
    ~Chord();
    void set_peers(const std::map<int, std::pair<std::string, int> > & table);
    int set(std::string key, std::string value);
    std::string get(std::string key);
    std::vector<int> owner(std::string key);
    std::string list_local();
    const Unicast & get_unicast();

    void begin();
    void end();

private:
    int self_id;
    unsigned char self_hash;
    Unicast cast_helper;
    std::thread background_thrd;
    mutable pthread_mutex_t mutex;

    // need protection
    bool running;
    std::vector<std::pair<std::string, int> > finger_table;
    std::vector<std::pair<std::string, int> > successors;
    std::map<std::string, std::string> local_table;

    // functions
    unsigned char hash(unsigned char * char_arr, int length);
    void deamon();
};

#endif