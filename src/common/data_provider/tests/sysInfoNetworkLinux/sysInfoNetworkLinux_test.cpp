#include "sysInfoNetworkLinux_test.hpp"
#include "network/networkFamilyDataAFactory.h"
#include "network/networkInterfaceLinux.h"
#include <ifaddrs.h>

void SysInfoNetworkLinuxTest::SetUp() {};

void SysInfoNetworkLinuxTest::TearDown() {};

using ::testing::_; // NOLINT(bugprone-reserved-identifier)
using ::testing::Return;

class SysInfoNetworkLinuxWrapperMock : public INetworkInterfaceWrapper
{
public:
    SysInfoNetworkLinuxWrapperMock() = default;
    ~SysInfoNetworkLinuxWrapperMock() override = default;
    SysInfoNetworkLinuxWrapperMock(const SysInfoNetworkLinuxWrapperMock&) = delete;
    SysInfoNetworkLinuxWrapperMock& operator=(const SysInfoNetworkLinuxWrapperMock&) = delete;
    SysInfoNetworkLinuxWrapperMock(SysInfoNetworkLinuxWrapperMock&&) = delete;
    SysInfoNetworkLinuxWrapperMock& operator=(SysInfoNetworkLinuxWrapperMock&&) = delete;
    MOCK_METHOD(int, family, (), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(std::string, address, (), (const, override));
    MOCK_METHOD(std::string, netmask, (), (const, override));
    MOCK_METHOD(void, broadcast, (nlohmann::json&), (const, override));
    MOCK_METHOD(std::string, addressV6, (), (const, override));
    MOCK_METHOD(std::string, netmaskV6, (), (const, override));
    MOCK_METHOD(void, broadcastV6, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, gateway, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, metrics, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, metricsV6, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, dhcp, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, mtu, (nlohmann::json&), (const, override));
    MOCK_METHOD(LinkStats, stats, (), (const, override));
    MOCK_METHOD(void, type, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, state, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, MAC, (nlohmann::json&), (const, override));
    MOCK_METHOD(void, adapter, (nlohmann::json&), (const, override));
};

TEST_F(SysInfoNetworkLinuxTest, Test_AF_INET_THROW)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_INET));
    EXPECT_CALL(*mock, address()).Times(1).WillOnce(Return(""));
    EXPECT_ANY_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));
}

TEST_F(SysInfoNetworkLinuxTest, Test_AF_INET)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_INET));
    EXPECT_CALL(*mock, address()).Times(1).WillOnce(Return("192.168.0.1"));
    EXPECT_CALL(*mock, netmask()).Times(1).WillOnce(Return("255.255.255.0"));
    EXPECT_CALL(*mock, broadcast(_))
        .Times(1)
        .WillOnce([](nlohmann::json& json) { json["broadcast"] = "192.168.0.255"; });
    EXPECT_CALL(*mock, dhcp(_)).Times(1).WillOnce([](nlohmann::json& json) { json["dhcp"] = "8.8.8.8"; });
    EXPECT_CALL(*mock, metrics(_)).Times(1).WillOnce([](nlohmann::json& json) { json["metric"] = "100"; });
    EXPECT_NO_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));

    for (auto& element : ifaddr.at("IPv4"))
    {
        EXPECT_EQ("192.168.0.1", element.at("address").get_ref<const std::string&>());
        EXPECT_EQ("255.255.255.0", element.at("netmask").get_ref<const std::string&>());
        EXPECT_EQ("192.168.0.255", element.at("broadcast").get_ref<const std::string&>());
        EXPECT_EQ("8.8.8.8", element.at("dhcp").get_ref<const std::string&>());
        EXPECT_EQ("100", element.at("metric").get_ref<const std::string&>());
    }
}

TEST_F(SysInfoNetworkLinuxTest, Test_AF_INET6_THROW)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_INET6));
    EXPECT_CALL(*mock, addressV6()).Times(1).WillOnce(Return(""));
    EXPECT_ANY_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));
}

TEST_F(SysInfoNetworkLinuxTest, Test_AF_INET6)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_INET6));
    EXPECT_CALL(*mock, addressV6()).Times(1).WillOnce(Return("2001:db8:85a3:8d3:1319:8a2e:370:7348"));
    EXPECT_CALL(*mock, netmaskV6()).Times(1).WillOnce(Return("2001:db8:abcd:0012:ffff:ffff:ffff:ffff"));
    EXPECT_CALL(*mock, broadcastV6(_))
        .Times(1)
        .WillOnce([](nlohmann::json& json) { json["broadcast"] = "2001:db8:85a3:8d3:1319:8a2e:370:0000"; });
    EXPECT_CALL(*mock, dhcp(_)).Times(1).WillOnce([](nlohmann::json& json) { json["dhcp"] = "8.8.8.8"; });
    EXPECT_CALL(*mock, metricsV6(_)).Times(1).WillOnce([](nlohmann::json& json) { json["metric"] = "100"; });
    EXPECT_NO_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));

    for (auto& element : ifaddr.at("IPv6"))
    {
        EXPECT_EQ("2001:db8:85a3:8d3:1319:8a2e:370:7348", element.at("address").get_ref<const std::string&>());
        EXPECT_EQ("2001:db8:abcd:0012:ffff:ffff:ffff:ffff", element.at("netmask").get_ref<const std::string&>());
        EXPECT_EQ("2001:db8:85a3:8d3:1319:8a2e:370:0000", element.at("broadcast").get_ref<const std::string&>());
        EXPECT_EQ("8.8.8.8", element.at("dhcp").get_ref<const std::string&>());
        EXPECT_EQ("100", element.at("metric").get_ref<const std::string&>());
    }
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
TEST_F(SysInfoNetworkLinuxTest, Test_AF_PACKET)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_PACKET));
    EXPECT_CALL(*mock, name()).Times(1).WillOnce(Return("eth01"));
    EXPECT_CALL(*mock, adapter(_)).Times(1).WillOnce([](nlohmann::json& json) { json["adapter"] = "adapter"; });
    EXPECT_CALL(*mock, type(_)).Times(1).WillOnce([](nlohmann::json& json) { json["type"] = "ethernet"; });
    EXPECT_CALL(*mock, state(_)).Times(1).WillOnce([](nlohmann::json& json) { json["state"] = "up"; });
    EXPECT_CALL(*mock, MAC(_)).Times(1).WillOnce([](nlohmann::json& json) { json["mac"] = "00:A0:C9:14:C8:29"; });
    EXPECT_CALL(*mock, stats()).Times(1).WillOnce(Return(LinkStats {0, 1, 2, 3, 4, 5, 6, 7}));
    EXPECT_CALL(*mock, mtu(_)).Times(1).WillOnce([](nlohmann::json& json) { json["mtu"] = 1500; });
    EXPECT_CALL(*mock, gateway(_)).Times(1).WillOnce([](nlohmann::json& json) { json["gateway"] = "10.2.2.50"; });

    EXPECT_NO_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));

    EXPECT_EQ("eth01", ifaddr.at("name").get_ref<const std::string&>());
    EXPECT_EQ("adapter", ifaddr.at("adapter").get_ref<const std::string&>());
    EXPECT_EQ("ethernet", ifaddr.at("type").get_ref<const std::string&>());
    EXPECT_EQ("up", ifaddr.at("state").get_ref<const std::string&>());
    EXPECT_EQ("00:A0:C9:14:C8:29", ifaddr.at("mac").get_ref<const std::string&>());

    EXPECT_EQ(1, ifaddr.at("tx_packets").get<int32_t>());
    EXPECT_EQ(0, ifaddr.at("rx_packets").get<int32_t>());
    EXPECT_EQ(3, ifaddr.at("tx_bytes").get<int32_t>());
    EXPECT_EQ(2, ifaddr.at("rx_bytes").get<int32_t>());
    EXPECT_EQ(5, ifaddr.at("tx_errors").get<int32_t>());
    EXPECT_EQ(4, ifaddr.at("rx_errors").get<int32_t>());
    EXPECT_EQ(7, ifaddr.at("tx_dropped").get<int32_t>());
    EXPECT_EQ(6, ifaddr.at("rx_dropped").get<int32_t>());

    EXPECT_EQ(1500u, ifaddr.at("mtu").get<uint32_t>());
    EXPECT_EQ("10.2.2.50", ifaddr.at("gateway").get_ref<const std::string&>());
}

TEST_F(SysInfoNetworkLinuxTest, Test_AF_UNSPEC_THROW_NULLPTR)
{
    nlohmann::json ifaddr {};
    EXPECT_ANY_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(nullptr)->buildNetworkData(ifaddr));
}

TEST_F(SysInfoNetworkLinuxTest, Test_Gateway_7546)
{
    auto mock {std::make_shared<SysInfoNetworkLinuxWrapperMock>()};
    nlohmann::json ifaddr {};
    EXPECT_CALL(*mock, family()).Times(1).WillOnce(Return(AF_PACKET));
    EXPECT_CALL(*mock, name()).Times(1).WillOnce(Return("eth01"));
    EXPECT_CALL(*mock, adapter(_)).Times(1).WillOnce([](nlohmann::json& json) { json["adapter"] = "adapter"; });
    EXPECT_CALL(*mock, type(_)).Times(1).WillOnce([](nlohmann::json& json) { json["type"] = "ethernet"; });
    EXPECT_CALL(*mock, state(_)).Times(1).WillOnce([](nlohmann::json& json) { json["state"] = "up"; });
    EXPECT_CALL(*mock, MAC(_)).Times(1).WillOnce([](nlohmann::json& json) { json["mac"] = "00:A0:C9:14:C8:29"; });
    EXPECT_CALL(*mock, stats()).Times(1).WillOnce(Return(LinkStats {0, 1, 2, 3, 4, 5, 6, 7}));
    EXPECT_CALL(*mock, mtu(_)).Times(1).WillOnce([](nlohmann::json& json) { json["mtu"] = 1500; });
    EXPECT_CALL(*mock, gateway(_))
        .Times(1)
        .WillOnce([](nlohmann::json& json) { json["gateway"] = "A12BA8C0"; }); // Gateway value in hexa: A12BA8C0

    EXPECT_NO_THROW(FactoryNetworkFamilyCreator<OSPlatformType::LINUX>::create(mock)->buildNetworkData(ifaddr));

    EXPECT_EQ("eth01", ifaddr.at("name").get_ref<const std::string&>());
    EXPECT_EQ("adapter", ifaddr.at("adapter").get_ref<const std::string&>());
    EXPECT_EQ("ethernet", ifaddr.at("type").get_ref<const std::string&>());
    EXPECT_EQ("up", ifaddr.at("state").get_ref<const std::string&>());
    EXPECT_EQ("00:A0:C9:14:C8:29", ifaddr.at("mac").get_ref<const std::string&>());

    EXPECT_EQ(1, ifaddr.at("tx_packets").get<int32_t>());
    EXPECT_EQ(0, ifaddr.at("rx_packets").get<int32_t>());
    EXPECT_EQ(3, ifaddr.at("tx_bytes").get<int32_t>());
    EXPECT_EQ(2, ifaddr.at("rx_bytes").get<int32_t>());
    EXPECT_EQ(5, ifaddr.at("tx_errors").get<int32_t>());
    EXPECT_EQ(4, ifaddr.at("rx_errors").get<int32_t>());
    EXPECT_EQ(7, ifaddr.at("tx_dropped").get<int32_t>());
    EXPECT_EQ(6, ifaddr.at("rx_dropped").get<int32_t>());
    EXPECT_EQ(1500, ifaddr.at("mtu").get<int32_t>());
    EXPECT_EQ("A12BA8C0", ifaddr.at("gateway").get_ref<const std::string&>());
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
