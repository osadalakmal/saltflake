#include "saltflakeimpl.h"
#include "saltflake.h"
#include "byteswap_compat.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>

#include <cassert>
#include <memory>
#include <thread>

namespace saltflake {

namespace {

template <typename T, T val>
std::pair<uint16_t, bool> getLowerNBitsOfIP(
    const IfAddrsPopulator* ifsAddrsPopulator) {
    ifaddrs* ifAddrStruct = nullptr;
    auto errCode = ifsAddrsPopulator ? getifaddrs(&ifAddrStruct)
                                     : (*ifsAddrsPopulator)(&ifAddrStruct);
    assert(errCode == 0);
    auto ifaDeleter = [](ifaddrs* ifa) {
        if (ifa)
            freeifaddrs(ifa);
    };
    std::unique_ptr<ifaddrs, decltype(ifaDeleter)> interfaceAddrs(ifAddrStruct,
                                                                  ifaDeleter);

    for (auto ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET &&
            !(IFF_LOOPBACK & ifa->ifa_flags)) {
            const auto addrBinForm = __bswap_32(
                reinterpret_cast<sockaddr_in*>(ifa->ifa_addr)->sin_addr.s_addr);
            auto mask = (1U << (val * 2)) - 1;
            return {(addrBinForm & (mask)), true};
        }
    }
    return {0, false};
}

}  // anonymous namespace

template <typename T>
SaltflakeImpl<T>::SaltflakeImpl(const SaltFlakeSettings& sfSettings)
    : m_mutex(),
      m_startTime(T::now()),
      m_elapsedTime(T::duration::zero()),
      m_sequence(0) {
    auto retVal = getLowerNBitsOfIP<int, 12>(&(sfSettings.ifaddrsPopulator));
    if (!retVal.second) {
        throw std::runtime_error("Could not create saltflake generator object");
    }
    m_machineId = retVal.first;
}

template <typename T>
uint64_t SaltflakeImpl<T>::nextId() {
    constexpr auto maskSequence = static_cast<uint16_t>((1 << SEQUENCE_BIT_LEN) - 1);
    std::lock_guard<std::mutex> g(m_mutex);

    auto current = T::now() - m_startTime;
    if (m_elapsedTime < current) {
        m_elapsedTime = current;
        m_sequence = 0;
    } else {
        m_sequence = (m_sequence + 1) & maskSequence;
        if (m_sequence == 0) {
            m_elapsedTime++;
            const auto overtime = m_elapsedTime - current;
            std::this_thread::sleep_for(overtime);
        }
    }
    return getId();
}

template <typename T>
uint64_t SaltflakeImpl<T>::getId() {
    return uint64_t(m_elapsedTime.count()) << (SEQUENCE_BIT_LEN + MACHINEID_BIT_LEN) |
           uint64_t(m_sequence) << MACHINEID_BIT_LEN |
           uint64_t(m_machineId);
}

template <typename T>
SaltflakeImpl<T>::~SaltflakeImpl() {}

template <typename T>
std::vector<uint64_t> SaltflakeImpl<T>::decompose(uint64_t id) {
    constexpr uint64_t maskSequence = (0xFFFFFFFFFFFFFFFF >> (64 - SEQUENCE_BIT_LEN)) << MACHINEID_BIT_LEN;
    constexpr uint64_t maskMachineID = 0xFFFFFFFFFFFFFFFF >> (64 - MACHINEID_BIT_LEN);

    std::vector<uint64_t> retval(4);
    retval[ID_INDEX] = id;
    retval[TIME_INDEX] = id >> (SEQUENCE_BIT_LEN + MACHINEID_BIT_LEN);
    retval[SEQUENCE_INDEX] = (id & maskSequence) >> MACHINEID_BIT_LEN;
    retval[MACHINE_ID_INDEX] = id & maskMachineID;
    return retval;
}

}  // namespace saltflake
