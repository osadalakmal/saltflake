#include <gtest/gtest.h>

#include "io/server.h"
#include "testclient.h"

using namespace ::testing;

namespace saltflake {

class ServerTest : public ::testing::Test {
public:
    ServerTest()
        : m_server("/tmp/server_test.socket"),
          m_client("/tmp/server_test.socket") {}
    virtual ~ServerTest() {}

protected:
    tcb::span<char> readCb(const tcb::span<char> buf) {
        return buf;
    }

    io::Server m_server;
    TestClient m_client;
};

}  // namespace saltflake