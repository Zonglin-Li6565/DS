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
    int tmp;
    read >> tmp;    
}