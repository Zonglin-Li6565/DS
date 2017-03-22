#include <iostream>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <fstream>
#include "chord.h"
#include "exception.h"

int main(int argc, char* argv[]) {
    /**
     * call with a.out id
     */
    int id = std::stoi(argv[1]);
    std::ifstream read;
    read.open(argv[2], std::ios::in);
    int num_nodes;
    read >> num_nodes;
    std::map<int, std::pair<std::string, int> > table;
    for (int i = 0; i < num_nodes; i++) {
        int id, port;
        std::string ipaddr;
        read >> id >> ipaddr >> port;
        table[id] = std::make_pair(ipaddr, port);
    }   
    Chord chord(id);
    chord.set_peers(table);
    chord.begin();
    while(1) {
        std::string s;
        std::cin >> s;
        if (s == "list") {
            std::cout << chord.list_local() << std::endl;
        } else if (s == "set") {
            std::string key, val;
            std::cin >> key >> val;
            chord.set(key, val);
            std::cout << "set successful" << std::endl;
        } else if (s == "get") {
            std::string key;
            std::cin >> key;
            try{
                std::cout << "found: " << chord.get(key) << std::endl;
            } catch(int e) {
                std::cout << "not found" << std::endl;
            }
        }
    }
}