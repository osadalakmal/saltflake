#include <gtest/gtest.h>
#include "io/server.h"
#include <mqueue.h>

using namespace ::testing;

namespace saltflake {

void deletemq(const char* queueName) {
    mq_unlink(queueName);
}

class MQClient {
public:
    void sendMessage() {
        struct mq_attr attr = {};

        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = Server::MSG_SIZE;
        attr.mq_curmsgs = 0;

        auto m_mq = mq_open("/TEST_QUEUE", O_CREAT | O_WRONLY, 0644, &attr);
        const char buf[] = "Hello";
        mq_send(m_mq, buf, strlen("Hello"), 0);
        mq_close(m_mq);
    }
};

class ServerTest : public ::testing::Test {
public:
    ServerTest() : m_server(), m_client() {
        deletemq("/TEST_QUEUE");
    }
    virtual ~ServerTest() {
        deletemq("/TEST_QUEUE");
    }

protected:
    Server m_server;
    MQClient m_client;
};

TEST_F(ServerTest, Open_works) {
    auto rc = m_server.open("/TEST_QUEUE");
    ASSERT_TRUE(rc);
}

TEST_F(ServerTest, Recv_Works) {
    m_server.open("/TEST_QUEUE");
    m_client.sendMessage();
    char data[Server::MSG_SIZE] = {};
    tcb::span<char> buf(data, Server::MSG_SIZE);
    auto rc = m_server.recv(buf);
    ASSERT_EQ(true, rc);
    ASSERT_STREQ("Hello", data);
}

TEST_F(ServerTest, Send_Works) {
    char data[Server::MSG_SIZE] = {};
    strncpy(data, "Hello World!", strlen("Hello World!"));
    tcb::span<char> buf(data, Server::MSG_SIZE);
    m_server.open("/TEST_QUEUE");
    m_server.send(buf);
    auto rc = m_server.recv(buf);
    ASSERT_EQ(true, rc);
    ASSERT_STREQ("Hello World!", data);
}

} // namespace saltflake