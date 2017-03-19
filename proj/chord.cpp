#include "chord.h"
#include <cstring>

static unsigned char ran_table[256] = {
    20,  92,  47,  96,   6, 104,  41,  31,  38,  23, 127,  98,  22,
    79,  16,   9, 113,  62,  29,  65,  67,  69,  76,  52,  99,  70,
    85,  95,  19,  54,  94,   3,  53,  12,  81,  34,   0,  61,  55,
    37,   8, 121,  40, 112,  18, 109,  35, 125,  56,  59,  74,  30,
    117,  24,  87,  33,  14,  89,  25,  66,  46,  77,  51, 120, 107,
     4,  42, 122, 108,   5,  26,  28,  27,  91,  71,  90,  39, 119,
    78, 111,  44,  45, 115,  80,  72,   1,  15, 106,  88, 101, 100,
    10, 110,  57,  58,  49,  36,   7,  63,  32, 118, 124,  73,  50,
    17,  93,  86,  48, 103,  11,  21,  97, 123,  82,   2,  64,  84,
    116, 126, 114,  60, 102,  68, 105,  13,  75,  43,  83,
    79,  48,  78,   1,  26,  69,  70,  73, 105,  31,  23,  89, 112,
    10,  98,  80,  99, 113,  12,  21,  57,  95,  61,  76,  83,  25,
    108,  63,  42,  66, 124,  47, 100,  93, 102,  74,  19,  18,  88,
    58,   9,  81,  55, 109,  59,  11, 115,  13,  97,  68,  22,  82,
    119,  30, 121,  71,  96,  51,  56, 114, 126,  85,  91,  92,  46,
    45,  77,  65, 103,  33,   2,  17,  72,   3,  27,  52,  28,  53,
    94,  44,  84,  60,  40,  43,  41,  90,  15, 111,  39,   4,  64,
    32, 110, 125, 122, 123,   0,  36, 120, 101, 116, 104,   6,  20,
    50,  75,  54,  34,   5,  37,  87,  49,  62, 127,  86,  16, 107,
    118,  14,  38, 117,  35,   7,  24,   8, 106,  29,  67
};

void Chord::set_peers(const std::map<int, std::pair<std::string, int> > & table) {
    // At most 128 peers
    const std::pair<std::string, int> * lookup[MAX_NUM_PEERS];
    memset(lookup, 0, sizeof(lookup));
    for (auto it = table.begin(); it != table.end(); it ++) {
        lookup[it->first] = &(it->second);
    }
    for (int i = 0; (1 << i) < MAX_NUM_PEERS; i++) {
        int idx = self_id + (1 << i);
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
}

int Chord::set(std::string key, std::string value) {
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

unsigned char Chord::hash(std::string str) {
    const char * char_arr = str.c_str();
    unsigned char h = 0;
    for (int i = 0; i < str.size(); i++) {
        h = ran_table[h ^ (char_arr[i])];
    }
    return h;
}
