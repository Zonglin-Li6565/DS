#include "fd.h"

void set_owner(FDInterface * ptr) {
    owner = ptr;
}

std::map<ProcDesc, bool> get_status() {
    return std::map<ProcDesc, bool>();
}

void set_unicast(Unicast * unicast) {

}

void set_targets(const std::vector<ProcDesc> & targets) {

}

void set_trackers(const std::vector<ProcDesc> & trackers) {

}

void start() {
    if (owner == NULL) {
        throw NULL_POINTER;
    }
}

void end() {

}
