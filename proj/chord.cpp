#include "chord.h"
#include <cstring>

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
