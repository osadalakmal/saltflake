#ifndef SALTFLAKE_SALTFLAKE_H
#define SALTFLAKE_SALTFLAKE_H

#include <chrono>
#include <mutex>

typedef int (*IfAddrsPopulator) (struct ifaddrs **);

namespace saltflake {

template<typename T>
class SaltflakeImpl;

struct SaltFlakeSettings {
  IfAddrsPopulator ifaddrsPopulator;
};

class Saltflake {
    Saltflake(const SaltFlakeSettings& sfSettings);
    virtual ~Saltflake();
private:
  SaltflakeImpl<std::chrono::steady_clock>* pImpl;
};

} // namespace saltflake

#endif //SALTFLAKE_SALTFLAKE_H
