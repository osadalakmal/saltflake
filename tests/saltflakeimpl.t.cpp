#include <gtest/gtest.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <saltflake.h>
#include <saltflakeimpl.cpp>

using namespace ::testing;

namespace saltflake {

class FakeSameMilliClock {
public:
    typedef std::chrono::milliseconds duration;
    typedef duration::rep rep;
    typedef duration::period period;
    typedef std::chrono::time_point<FakeSameMilliClock> time_point;
    static const bool is_steady = true;
    static int timeNow;

    static time_point now() noexcept
    {
        using namespace std::chrono;
        return time_point(duration(timeNow));
    }

    static void setTimeNow(int now) {
        timeNow = now;
    }
};

int FakeSameMilliClock::timeNow = 1;

class SaltflakeImplTest : public ::Test {
public:
    SaltflakeImplTest() {
        SaltFlakeSettings sfSettings;
        sfSettings.ifaddrsPopulator = [](struct ifaddrs** ifs) -> int {
            *ifs = static_cast<ifaddrs*>(malloc(sizeof(ifaddrs)));
            static sockaddr_in sin;
            sin.sin_addr.s_addr = 2130706433;
            (*ifs)->ifa_addr = reinterpret_cast<sockaddr*>(&sin);
            (*ifs)->ifa_next == nullptr;
            return 0;
        };
        m_saltflake.reset(new SaltflakeImpl<>(sfSettings));
        m_fakeSaltflake.reset(new SaltflakeImpl<FakeSameMilliClock>(sfSettings));
    }

protected:
    std::unique_ptr<SaltflakeImpl<std::chrono::steady_clock>> m_saltflake;
    std::unique_ptr<SaltflakeImpl<FakeSameMilliClock>> m_fakeSaltflake;

};

TEST_F(SaltflakeImplTest, Getting_An_IP_Address_Works) {
    ASSERT_EQ(264, m_saltflake->m_machineId);
}

TEST_F(SaltflakeImplTest, Id_increases_monotomically) {
    auto old_id = 0ULL;
    for (int i = 0; i < 10000; i++) {
        auto new_id = m_saltflake->nextId();
        ASSERT_GT(new_id, old_id);
        old_id = new_id;
    }
}

TEST_F(SaltflakeImplTest, Id_decompose_works) {
    auto id = m_fakeSaltflake->nextId();
    auto parts = SaltflakeImpl<>::decompose(id);
    ASSERT_EQ(id, parts[SaltflakeImpl<>::ID_INDEX]);
    ASSERT_EQ(0, parts[SaltflakeImpl<>::TIME_INDEX]);
    ASSERT_EQ(0, parts[SaltflakeImpl<>::SEQUENCE_INDEX]);
    ASSERT_EQ(264, parts[SaltflakeImpl<>::MACHINE_ID_INDEX]);
}

TEST_F(SaltflakeImplTest, Sequence_increases_correctly) {
    auto id1 = m_fakeSaltflake->nextId();
    auto parts1 = SaltflakeImpl<>::decompose(id1);
    auto id2 = m_fakeSaltflake->nextId();
    auto parts2 = SaltflakeImpl<>::decompose(id2);
    ASSERT_EQ(parts1[SaltflakeImpl<>::SEQUENCE_INDEX] + 1, parts2[SaltflakeImpl<>::SEQUENCE_INDEX]);
}

}  // namespace saltflake