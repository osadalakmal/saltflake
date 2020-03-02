#ifndef SALTFLAKE_SALTFLAKE_H
#define SALTFLAKE_SALTFLAKE_H

#include <chrono>
#include <future>
#include <mutex>
#include <vector>

#include "io/server.h"
#include "tcb_span.h"

typedef int (*IfAddrsPopulator) (struct ifaddrs **);

namespace saltflake {

template<typename T>
class SaltflakeImpl;

struct SaltFlakeSettings {
  IfAddrsPopulator ifaddrsPopulator;
  std::string queueName;
  int maxConnections;
};

class Saltflake {
public:
    Saltflake(const SaltFlakeSettings& sfSettings);
    tcb::span<char> handleMessage(const tcb::span<char>& buf);
    virtual ~Saltflake();
private:
  SaltflakeImpl<std::chrono::steady_clock>* pImpl;
  io::Server m_server;
  std::future<int> m_serverReturnVal;
};

} // namespace saltflake

#endif //SALTFLAKE_SALTFLAKE_H
