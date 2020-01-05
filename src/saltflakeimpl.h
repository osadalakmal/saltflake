#ifndef SALTFLAKE_SALTFLAKEIMPL_H
#define SALTFLAKE_SALTFLAKEIMPL_H

#include <mutex>
#include <vector>

namespace saltflake {

class SaltFlakeSettings;

constexpr auto TIMESTAMP_BIT_LEN = 39; // bit length of time
constexpr auto MACHINEID_BIT_LEN = 12; // bit length of machine id
constexpr auto SEQUENCE_BIT_LEN  = 64 - TIMESTAMP_BIT_LEN - MACHINEID_BIT_LEN;    // bit length of sequence number

template <typename T = std::chrono::steady_clock>
class SaltflakeImpl {
   public:
    SaltflakeImpl(const SaltFlakeSettings& sfSettings);
    virtual ~SaltflakeImpl();

    uint64_t nextId();
    uint64_t getId();
    static std::vector<uint64_t> decompose(uint64_t id);

    static constexpr auto ID_INDEX = 0;
    static constexpr auto TIME_INDEX = 1;
    static constexpr auto SEQUENCE_INDEX = 2;
    static constexpr auto MACHINE_ID_INDEX = 3;

    std::mutex m_mutex;
    typename T::time_point m_startTime;
    typename T::duration m_elapsedTime;
    uint16_t m_sequence;
    uint16_t m_machineId;
};

template class SaltflakeImpl<std::chrono::steady_clock>;

}  // namespace saltflake

#endif  // SALTFLAKE_SALTFLAKEIMPL_H
