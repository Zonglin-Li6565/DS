#include "fd.h"
#include "exception.h"

void PingAck::set_owner(FDInterface * ptr) {
    owner = ptr;
}

std::map<ProcDesc, bool> PingAck::get_status() {
    return std::map<ProcDesc, bool>();
}

void PingAck::set_unicast(Unicast * unicast) {

}

void PingAck::set_targets(const std::vector<ProcDesc> & targets) {

}

void PingAck::start() {
    if (owner == NULL) {
        throw NULL_POINTER;
    }
}

void PingAck::end() {

}
