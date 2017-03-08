#include <string>
#include <vector>

namespace structs {
    
    struct receiver_struct {
        pthread_mutex_t lock;
        void * owner;
        bool stillOn;
    };

    struct sender_struct {
        pthread_mutex_t lock;
        void * owner;
    };

}

class Unicast {
public:

    Unicast (int portnum);

    Unicast (int portnum, int max_delay);

    void send (std::string msg, std::string ipaddr, int port);

    int get_port();

    bool running();

    void stop();

    std::string deliever ();

    void message_arrives(std::string msg);

    int get_delay_bound();

private:
    int port;

    std::vector<std::string> sendbuf;
    std::string rec_msg;

    int delay_bound;        // delay in ms

    pthread_t server_thrd;
    std::vector<pthread_t> sender_thrds;

    volatile bool terminated;

    pthread_cond_t wait;
    pthread_mutex_t mutex;
};