#ifndef SALTFLAKE_SERVER_H
#define SALTFLAKE_SERVER_H

#include "tcb_span.h"
#include <mqueue.h>

namespace saltflake {

class Server {
public:
    bool open(const char* queueName);
    bool recv(tcb::span<char>& buf);
    bool send(const tcb::span<char>& data);

    static constexpr int MSG_SIZE = 255;
private:
    mqd_t m_mq;
};

} // namespace saltflake

#endif  // SALTFLAKE_SERVER_H
