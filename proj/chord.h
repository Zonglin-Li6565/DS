#ifndef CHORD_H
#define CHORD_H

#include <string>
#include <vector>
#include <map>
#include <utility>

# define MAX_NUM_PEERS 128

class Chord {
public:
    Chord(int pid) : self_id(pid){};
    void set_peers(const std::map<int, std::pair<std::string, int> > & table);
    int set(std::string key, std::string value);
    std::string get(std::string key);
    std::vector<int> owner(std::string key);
    std::string list_local();

private:
    int self_id;
    std::vector<std::pair<std::string, int> > finger_table;
    std::map<std::string, std::string> local_table;

    unsigned char hash(std::string);
};

#endif