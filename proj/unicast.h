#ifndef UNICAST_H
#define UNICAST_H

#include <string>
#include <vector>
#include <map>

class Unicast {
public:

    /**
     * Constructor 
     * @param portnum   the port number
     */
    Unicast (int portnum);

    /**
     * Constructor
     * @param portnum   the port number
     * @param max_delay the maximum delay for simulating the network
     */
    Unicast (int portnum, int max_delay);

    /**
     * Send a message to a specified host. Thread safe
     * @param tag       the tag for the message
     * @param msg       the message body
     * @param ipaddr    the ip address
     * @param port      port number
     * @return 0 if success, -1 if failed.
     */
    int send (std::string tag, std::string msg, std::string ipaddr, int port);

    /**
     * Returns the port information. Thread safe
     * @return the current port number of this instance
     */
    int get_port();

    /**
     * Check whether the owner want to terminate. Thread safe
     * @return true if terminated; false otherwise
     */
    bool running();

    /**
     * Stop the unicast. Thread safe
     */
    void stop();

    /**
     * For user threads to call. Will block until the process arrives. 
     * Safe for multiple thread on the same tag.
     * @param tag   the tag of the message currently waiting for
     * @return the message body (without tag)
     */
    std::string deliever (std::string tag);

    /**
     * Public helper function. Called by the internal threads
     * @param msg   the raw mesage
     */
    void message_arrives(std::string msg);

    /**
     * Get the delay bound.
     * @return the delay bound
     */
    int get_delay_bound();

private:
    int port;

    std::vector<std::string> sendbuf;
    std::string rec_msg;

    int delay_bound;        // delay in ms

    pthread_t server_thrd;
    std::vector<pthread_t> sender_thrds;

    volatile bool terminated;

    std::map<std::string, pthread_cond_t> wait_conds;
    pthread_mutex_t mutex;

    int send (std::string msg, std::string ipaddr, int port);
};

#endif