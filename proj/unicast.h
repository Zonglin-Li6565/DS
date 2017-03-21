#ifndef UNICAST_H
#define UNICAST_H

#include <string>
#include <vector>
#include <map>
#include <pthread.h>
#include <regex>

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
     * @param host_ip   the ip address
     * @param host_port port number
     * @return 0 if success; -1 if failed to connect server; -2 if failed
     *         to open the socket
     */
    int send (std::string tag, std::string msg, std::string host_ip, int host_port) const;

    /**
     * Returns the port information. Thread safe
     * @return the current port number of this instance
     */
    int get_port() const;

    /**
     * Check whether the owner want to terminate. Thread safe
     * @return true if terminated; false otherwise
     */
    bool running() const;

    /**
     * Start the server thread
     */
    void begin();

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
    std::string deliever (std::string tag) const;

    /**
     * Wait for the message to arrive. With timeout. Throws exception on time out
     * @param  tag        the tag of the message currently waiting for
     * @param  timeout_ms milliseconds timeout (set to -1 for no timeout)
     * @return            the message body (without tag)
     */
    std::string deliever (std::string tag, int timeout_ms) const;

    /**
     * Public helper function. Called by the internal threads
     * @param msg   the raw mesage
     */
    void message_arrives(std::string msg) const;

    /**
     * Get the delay bound.
     * @return the delay bound
     */
    int get_delay_bound() const;

private:
    int port;

    std::vector<std::string> sendbuf;
    mutable std::string rec_msg;

    int delay_bound;        // delay in ms

    pthread_t server_thrd;
    std::vector<pthread_t> sender_thrds;

    volatile bool terminated;

    mutable std::map<std::string, pthread_cond_t> wait_conds;
    mutable pthread_mutex_t mutex;

    const std::regex expression;
};

#endif