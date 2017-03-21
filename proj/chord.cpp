#include "chord.h"
#include <cstring>

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

void Chord::set_peers(const std::map<int, std::pair<std::string, int> > & table) {
    // At most 256 peers
    // input is a map from id to pair <id addr, port>
    const std::pair<std::string, int> * lookup[MAX_NUM_PEERS];
    memset(lookup, 0, sizeof(lookup));
    for (auto it = table.begin(); it != table.end(); it ++) {
        unsigned char * id = (unsigned char *)&(it->first);
        lookup[hash(id, 4)] = &(it->second);
    }
    // fill the finger table
    for (int i = 0; (1 << i) < MAX_NUM_PEERS; i++) {
        int idx = (self_hash + (1 << i)) % MAX_NUM_PEERS;
        if (lookup[idx] == NULL) {
            for (int j = idx - 1; j >= 0; j --) {
                if (lookup[j] != NULL) {
                    finger_table.push_back(*lookup[j]);
                }
            }
        } else {
            finger_table.push_back(*lookup[idx]);
        }
    }
    // find successors
    for (int i = 0, j = 0; i < MAX_NUM_PEERS && j < 2; i ++) {
        int idx = (i + self_hash) % MAX_NUM_PEERS;
        if (lookup[idx] != NULL) {
            j ++;
            successors.push_back(*lookup[j]);
        }
    }
    cast_helper.begin();    // start the server thread
}

int Chord::set(std::string key, std::string value) {
    unsigned char h = hash((unsigned char *)key.c_str(), key.size());

    if (h == self_hash) {               // just store.
        local_table[key] = value;
        return 0;
    }

    
    return -1;
}

std::string Chord::get(std::string key) {
    return "";
}

std::vector<int> Chord::owner(std::string key) {
    return std::vector<int>();
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
    pthread_mutex_lock(&mutex);
    running = false;
    pthread_mutex_unlock(&mutex);
    background_thrd.join();
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
        
    }
}
