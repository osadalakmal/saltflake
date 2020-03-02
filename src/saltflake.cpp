#include "saltflake.h"

#include <sys/socket.h>

#include <chrono>
#include <cstring>
#include <functional>

#include "io/messageparser.h"
#include "saltflakeimpl.h"

namespace saltflake {

namespace {}  // anonymous namespace

Saltflake::Saltflake(const SaltFlakeSettings& sfSettings)
    : pImpl(new SaltflakeImpl<std::chrono::steady_clock>(sfSettings)),
      m_server(
          sfSettings.queueName.empty() ? "/tmp/saltflake.server"
                                       : sfSettings.queueName.c_str()) {
    m_serverReturnVal = std::async(std::launch::async, [&]() {
        return m_server.run();
    });
}

Saltflake::~Saltflake() {
    m_server.close();
    m_serverReturnVal.wait();
    delete pImpl;
}

tcb::span<char> Saltflake::handleMessage(const tcb::span<char>& buf) {
    tcb::span<char> sendData(reinterpret_cast<char*>(malloc(1)), 1);
    io::MessageParser messageParser;
    io::MessageParser::OP_CODE opCode = messageParser.getOpCodeFromBuf(buf);
    if (opCode == io::MessageParser::OP_CODE::ERROR) {
        return tcb::span<char>();
    } else if (opCode == io::MessageParser::OP_CODE::GET) {
        auto id = std::to_string(pImpl->getId());
        memccpy(&sendData[0], id.c_str(), 1, id.size());
        return sendData;
    } else if (opCode == io::MessageParser::OP_CODE::EXIT) {
        return tcb::span<char>();
    }
}

}  // namespace saltflake