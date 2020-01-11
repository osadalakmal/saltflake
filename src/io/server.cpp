#include "server.h"

#include <mqueue.h>

#include <cstddef>
#include <cstring>
#include <iostream>

#include "../tcb_span.h"

#define MSG_STOP    "exit"

namespace saltflake {

bool Server::open(const char* queueName) {
    struct mq_attr attr = {};

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;

    m_mq = mq_open(queueName, O_CREAT | O_RDWR, 0644, &attr);
    if ((mqd_t)-1 == m_mq) {
        // TODO Log error
        std::cout << "Errno is " << errno << std::endl;
        return false;
    }
    return true;
}

bool Server::recv(tcb::span<char>& buf) {
    ssize_t bytes_read;
    bytes_read = mq_receive(m_mq, buf.begin(), buf.size(), nullptr);
    if (bytes_read < 0) {
        return false;
    }
    buf[bytes_read] = '\0';
    return true;
}

bool Server::send(const tcb::span<char>& data) {
    auto rc = mq_send(m_mq, data.cbegin(), data.size(), 0);
    return rc == 0;
}

} // namespace saltflake
