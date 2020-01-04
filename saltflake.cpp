#include "saltflake.h"
#include "saltflakeimpl.h"

#include <chrono>

namespace saltflake {

namespace {

}  // anonymous namespace

Saltflake::Saltflake(const SaltFlakeSettings& sfSettings) : pImpl(new SaltflakeImpl<std::chrono::steady_clock>(sfSettings)) {
}

}  // namespace saltflake