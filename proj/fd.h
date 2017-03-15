/**
 * Failure detector
 */

#ifndef FD_H
#define FD_H

#include <map>
#include <string>
#include <vector>

#include "unicast.h"

class ProcDesc {
public:
    const std::string ipaddr;
    const int portnum;

    ProcDesc(std::string ipadd, int port) : ipaddr(ipadd), portnum(port){};
};

class FDInterface {
public:
    virtual void failed_callback(std::vector<ProcDesc>) = 0;
};

class FailureDetector {
public:
    virtual void set_owner(FDInterface * ptr) = 0;
    virtual std::map<ProcDesc, bool> get_status() = 0;
    virtual void set_unicast(Unicast * unicast) = 0;
};

class HeartBeat : public FailureDetector {
public:

};

class PingAck : public FailureDetector {
public:

};

#endif
