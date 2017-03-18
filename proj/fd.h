/**
 * Failure detector
 */

#ifndef FD_H
#define FD_H

#include <map>
#include <string>
#include <vector>

#include "unicast.h"

#define PINGACK_TAG     "pingack"
#define HEARTBEAT_TAG   "heartbeat"

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
    virtual void set_targets(std::vector<ProcDesc> targets) = 0;
    virtual void start() = 0;
    virtual void end() = 0;
};

class HeartBeat : public FailureDetector {
public:
    void set_owner(FDInterface * ptr);
    std::map<ProcDesc, bool> get_status();
    void set_unicast(Unicast * unicast);
    void set_targets(const std::vector<ProcDesc> & targets);
    void set_trackers(const std::vector<ProcDesc> & trackers);
    void start();
    void end();
private:
    FDInterface * owner;
    Unicast * unicast;
    std::vector<ProcDesc> targets;
    std::vector<ProcDesc> trackers;
};

class PingAck : public FailureDetector {
public:
    void set_owner(FDInterface * ptr);
    std::map<ProcDesc, bool> get_status();
    void set_unicast(Unicast * unicast);
    void set_targets(const std::vector<ProcDesc> & targets);
    void start();
    void end();
private:
    FDInterface * owner;
    Unicast * unicast;
    std::vector<ProcDesc> targets;
};

#endif
