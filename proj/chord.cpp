#include <cstring>
#include <pthread.h>
#include "exception.h"
#include "chord.h"


#include <iostream>

#define getmatch(i, str, match) \
    str.substr(match.position(i) + 1, match.position(i) + match.length(i) - 2)

static unsigned char ran_table[MAX_NUM_PEERS] = {
    62,  34,  84,  25, 176, 217,  71, 201,  30,  44, 196,  88,  23,
    198, 250, 195, 174,  65, 113,   9, 249, 132, 253,  31,  57, 155,
    254,  40, 157,  94, 169, 154,  38, 210, 123,  48,  83, 184, 153,
    47,  36, 115,  98,   0, 118, 199, 102,  66,  20, 252,  77, 145,
    61, 127, 220, 131,  35, 120, 204, 111, 172,   2,  28, 138,  45,
    215, 105, 224, 112,   3,  37,  87,  12, 235, 180,  55,  19, 255,
     4, 185, 121, 207, 236,  79, 133, 140, 233, 119,  18,  46,  93,
    24, 141, 251,  92,  68, 219, 244, 228, 103,  67,  33, 107, 161,
    135, 136,  69,  64, 125,   8, 212, 134,  15, 240, 139,  10, 168,
    82, 137, 230, 152,  53, 104,  43, 109, 173,  85,  91, 129, 163,
    126, 187, 158, 164, 182, 197, 241,  78,  95, 130, 245, 165,  42,
    116, 248, 193, 246, 171, 177,  13, 232, 238,  22, 117, 148, 237,
    206,   7, 191,  29, 231,  80, 159,  74,  72,  56, 247,  51,  97,
    229,  21, 234, 181,   5, 150, 186,  59,  14, 208, 200,  49, 101,
    63, 160,  52, 156, 106,   1,  73,  90, 142,  16, 211,   6, 221,
    81, 225,  11,  41, 188, 128, 146, 194, 124, 203, 122, 213, 242,
    143, 226,  50, 205,  27, 170,  58, 239, 183, 162,  86, 110, 149,
    147, 144, 243, 209,  70,  96, 227,  99,  76,  39, 175, 216, 167,
    218, 151, 189, 222, 114,  32,  89, 108, 214,  26, 178, 190, 223,
    75, 100, 166,  17, 202,  60, 192,  54, 179
};

Chord::~Chord() {
    end();
    cast_helper.stop();
}

void Chord::set_peers(std::map<int, std::pair<std::string, int> > & table) {
    // At most 256 peers
    // input is a map from id to pair <ip addr, port>
    
    self_addr = table[self_id];

    const int * lookup[MAX_NUM_PEERS];
    memset(lookup, 0, sizeof(lookup));
    for (auto it = table.begin(); it != table.end(); it ++) {
        unsigned char * id = (unsigned char *)&(it->first);
        lookup[hash(id, 4)] = &it->first;
        std::cout << "id = " << (int)*id << " " << "hash = " << (int)hash(id, 4) << std::endl;
    }
    // fill the finger table
    for (int i = 0; (1 << i) < MAX_NUM_PEERS; i++) {
        int idx = (self_hash + (1 << i)) % MAX_NUM_PEERS;
        if (lookup[idx] == NULL) {
            for (int j = 0; j < MAX_NUM_PEERS; j ++) {
                if (lookup[(j + idx) % MAX_NUM_PEERS] != NULL) {
                    printf("%d is not null\n", (j + idx) % MAX_NUM_PEERS);
                    finger_table.push_back(std::make_pair((j + idx) % MAX_NUM_PEERS, 
                                            table[*lookup[(j + idx) % MAX_NUM_PEERS]]));
                }
            }
        } else {
            finger_table.push_back(std::make_pair(idx, table[*lookup[idx]]));
        }
    }
    // find successors
    for (int i = 0, j = 0; i < MAX_NUM_PEERS && j < NUM_SUCCESSORS; i ++) {
        int idx = (i + self_hash) % MAX_NUM_PEERS;
        if (lookup[idx] != NULL) {
            j ++;
            successors.push_back(std::make_pair(*lookup[idx], table[*lookup[idx]]));
        }
    }

    ////////test////////

    std::cout << "finger table: " << std::endl;
    for (auto a : finger_table) {
        std::cout << std::get<0>(a) << " " << std::get<0>(std::get<1>(a)) << " " << std::get<1>(std::get<1>(a)) << std::endl;
    }

    ////////////////////

    cast_helper.begin();    // start the server thread
}

int Chord::set(std::string key, std::string value) {
    std::string message = "<set><false><" + key + "><" + value 
                        + "><" + std::get<0>(self_addr) + "><" 
                        + std::to_string(std::get<1>(self_addr));
    pthread_mutex_lock(&mutex);
    cast_helper.send(CHORD_TAG, message, std::get<0>(self_addr), std::get<1>(self_addr));
    pthread_cond_wait(&set_cond, &mutex);
    pthread_mutex_unlock(&mutex);
    return 0;
}

std::string Chord::get(std::string key) {
    std::string message = "<get><false><" + key + "><"
                        + std::get<0>(self_addr) + "><" 
                        + std::to_string(std::get<1>(self_addr));
    pthread_mutex_lock(&mutex);
    cast_helper.send(CHORD_TAG, message, std::get<0>(self_addr), std::get<1>(self_addr));
    pthread_cond_wait(&get_cond, &mutex);
    if (get_success) {
        pthread_mutex_unlock(&mutex);
        return get_value;
    } else {
        pthread_mutex_unlock(&mutex);
        throw NOT_FOUND;
    }
    return "";
}

std::vector<int> Chord::owner(std::string key) {
    std::string message = "<get><false><" + key + "><"
                        + std::get<0>(self_addr) + "><" 
                        + std::to_string(std::get<1>(self_addr));
    pthread_mutex_lock(&mutex);
    cast_helper.send(CHORD_TAG, message, std::get<0>(self_addr), std::get<1>(self_addr));
    pthread_cond_wait(&get_cond, &mutex);
    if (get_success) {
        pthread_mutex_unlock(&mutex);
        return owners;
    } else {
        pthread_mutex_unlock(&mutex);
        throw NOT_FOUND;
    }
}

std::string Chord::list_local() {
    return "";
}

unsigned char Chord::hash(unsigned char * char_arr, int length) {
    unsigned char h = 0;
    for (int i = 0; i < length; i++) {
        h = ran_table[h ^ (char_arr[i])];
    }
    return h;
}

const Unicast & Chord::get_unicast() {
    return cast_helper;
}

void Chord::begin() {
    running = true;
    background_thrd = std::thread(std::bind(&Chord::deamon, this));
}

void Chord::end() {
    if (running) {
        pthread_mutex_lock(&mutex);
        running = false;
        pthread_mutex_unlock(&mutex);
        background_thrd.join();
    }
}

void Chord::deamon() {
    bool l_running;
    while (true) {
        pthread_mutex_lock(&mutex);
        l_running = running;
        pthread_mutex_unlock(&mutex);
        if (!l_running) {
            break;
        }
        // message format:
        //                type
        // looking:     <looking><key><value><(caller) ip><(caller) port>
        // set:         <set><end:true/false><key><value><(caller) ip><(caller) port>
        // get:         <get><end:true/false><key><(caller) ip><(caller) port>
        // setret:      <setret><true/false>
        // getret:      <getret><true/false><value><owner1id><owner2id>...
        std::string msg = cast_helper.deliever(CHORD_TAG);
        std::smatch match;
        if (match.empty()) {
            continue;
        }
        std::string type = getmatch(0, msg, match);
        if (type == "set") {
            if (match.size() != 6) {
                continue;
            }
            std::string key = getmatch(2, msg, match);
            unsigned char key_hash = hash((unsigned char *)key.c_str(), key.size());

            if (self_hash == key_hash || getmatch(1, msg, match) == "true") {       // just insert

                pthread_mutex_lock(&mutex);
                local_table[getmatch(2, msg, match)] = getmatch(3, msg, match);
                pthread_mutex_unlock(&mutex);

                cast_helper.send(CHORD_TAG, "<setret><true>", getmatch(4, msg, match), std::stoi(getmatch(5, msg, match)));            
            } else {

                unsigned int max = 0;
                std::pair<std::string, int> next;
                bool found = false;

                for (auto i : finger_table) {
                    unsigned int h = (unsigned int)std::get<0>(i);
                    if (h < key_hash && h > max) {
                        max = h;
                        next = std::get<1>(i);
                        found = true;
                    }
                }

                if (found) {
                    cast_helper.send(CHORD_TAG, msg, std::get<0>(next), std::get<1>(next));
                } else {
                    std::string message = std::string("<set>") + "<true><" + key + "><" + getmatch(3, msg, match) 
                                          + "><" + getmatch(4, msg, match) + "><" + getmatch(5, msg, match) + ">";
                    cast_helper.send(CHORD_TAG, message, std::get<0>(std::get<1>(successors[0])), std::get<1>(std::get<1>(successors[1])));
                }
            }
        } else if (type == "get") {
            if (match.size() != 5) {
                continue;
            }
            std::string key = getmatch(2, msg, match);
            unsigned char key_hash = hash((unsigned char *)key.c_str(), key.size());
            if (self_hash == key_hash || getmatch(1, msg, match) == "true") {       // just insert
                bool contains = false;
                std::string value;

                pthread_mutex_lock(&mutex);
                std::map<std::string, std::string>::iterator it = local_table.find(key);
                if (it != local_table.end()) {
                    value = local_table[key];
                    contains = true;
                }
                pthread_mutex_unlock(&mutex);

                std::string message;
                if (contains) {
                    message = "<getret><true><" + value + "><" + std::to_string(self_id) + ">";     // no backup
                } else {
                    message = "<getret><false>";
                }
                cast_helper.send(CHORD_TAG, message, getmatch(3, msg, match), std::stoi(getmatch(4, msg, match)));
            } else {
                unsigned int max = 0;
                std::pair<std::string, int> next;
                bool found = false;
                for (auto i : finger_table) {
                    unsigned int h = (unsigned int)std::get<0>(i);
                    if (h < key_hash && h > max) {
                        max = h;
                        next = std::get<1>(i);
                        found = true;
                    }
                }
                if (found) {
                    cast_helper.send(CHORD_TAG, msg, std::get<0>(next), std::get<1>(next));
                } else {
                    std::string message = std::string("<get>") + "<true><" + key + "><"
                                         + getmatch(3, msg, match) + "><" + getmatch(4, msg, match) + ">";
                    cast_helper.send(CHORD_TAG, message, std::get<0>(std::get<1>(successors[0])), std::get<1>(std::get<1>(successors[1])));
                }
            }
        } else if (type == "setret") {
            pthread_cond_broadcast(&set_cond);
        } else if (type == "getret") {
            pthread_mutex_lock(&mutex);
            if (match.size() < 4) {
                get_success = false;
            } else {
                get_success = getmatch(1, msg, match) == "true";
                get_value = getmatch(2, msg, match);
                owners.clear();
                for (int i = 3; i < match.size(); i++) {
                    owners.push_back(std::stoi(getmatch(i, msg, match)));
                }
            }
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&get_cond);
        }
    }
}
