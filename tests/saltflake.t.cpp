#include <gtest/gtest.h>
#include <saltflake.h>

#include <ifaddrs.h>
#include <io/server.h>
#include <netinet/in.h>
#include <thread>

#include "testclient.h"
using namespace ::testing;

namespace saltflake {

class SaltflakeTest : public ::Test {
public:
    SaltflakeTest() {
        SaltFlakeSettings sfSettings = {};
        sfSettings.queueName = "/tmp/server_test.socket";
        sfSettings.ifaddrsPopulator = [](struct ifaddrs** ifs) -> int {
          *ifs = static_cast<ifaddrs*>(malloc(sizeof(ifaddrs)));
          memset(*ifs, '\0', sizeof(ifaddrs));
          auto sin = new sockaddr_in();
          sin->sin_family = AF_INET;
          sin->sin_addr.s_addr = htonl(2130706433UL);
          ifs[0]->ifa_addr = reinterpret_cast<sockaddr*>(sin);
          ifs[0]->ifa_next == nullptr;
          return 0;
        };
        m_saltflake.reset(new Saltflake(sfSettings));
    }

protected:
    std::unique_ptr<Saltflake> m_saltflake;
    std::unique_ptr<TestClient> m_client;

};

TEST_F(SaltflakeTest, Server_Works) {
    struct stat buffer;
    while (stat ("/tmp/server_test.socket", &buffer) != 0) {
        usleep(10);
    }
    m_client.reset( new TestClient("/tmp/server_test.socket"));
    m_client->sendMessage();
    char data[64] = {};
    tcb::span<char> buf(data, 64);
    m_client->recvMessage(buf);
    std::cout << data << std::endl;
    m_client->sendExit();
}

} // namespace saltflake