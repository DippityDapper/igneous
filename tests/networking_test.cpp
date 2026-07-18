#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "igneous/networking/ClientConnectionWatchdog.hpp"
#include "igneous/networking/LocalIdentity.hpp"
#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkLoopbackLink.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"
#include "igneous/networking/NetworkProtocol.hpp"
#include "igneous/networking/PacketRouter.hpp"
#include "igneous/networking/PacketTypes.hpp"
#include "igneous/networking/Serializer.hpp"
#include "igneous/networking/NetworkSessionFactory.hpp"
#include "igneous/networking/ServerPeerActivityTracker.hpp"
#include "igneous/networking/SteamNetwork.hpp"

#include "enet/enet.h"

#include <filesystem>
#include <fstream>

#if __has_include(<bit>)
#include <bit>
#endif

using Catch::Matchers::WithinAbs;

struct ENetTestFixture
{
    ENetTestFixture()
    {
        REQUIRE(enet_initialize() == 0);
    }

    ~ENetTestFixture()
    {
        enet_deinitialize();
    }
};

TEST_CASE("Serializer round trip primitives", "[networking][serializer]")
{
    Engine::Serializer serializer;
    serializer.Write(true)
        .Write(static_cast<uint8_t>(7))
        .Write(static_cast<int32_t>(-42))
        .Write(static_cast<uint32_t>(9001))
        .Write(std::string("hello"))
        .Write(std::vector<uint8_t>{1, 2, 3});

    const std::vector<uint8_t> bytes = serializer.GetBytes();
    Engine::Deserializer deserializer(bytes, 0);

    REQUIRE(deserializer.ReadBool());
    REQUIRE(deserializer.ReadByte() == 7);
    REQUIRE(deserializer.ReadInt() == -42);
    REQUIRE(deserializer.ReadUInt() == 9001);
    REQUIRE(deserializer.ReadString() == "hello");
    REQUIRE(deserializer.ReadBytes() == std::vector<uint8_t>{1, 2, 3});
}

TEST_CASE("Packet header round trip through PacketRouter", "[networking][serializer][header]")
{
    Engine::Serializer serializer;
    Engine::NetworkProtocol::WritePacketHeader(serializer, static_cast<uint16_t>(Engine::PacketType::ChatMessagePacket));
    serializer.Write(std::string("payload"));

    const std::vector<uint8_t> bytes = serializer.GetBytes();
    REQUIRE(bytes.size() >= Engine::NetworkProtocol::HeaderSize);

    std::string receivedPayload;
    Engine::PacketRouter router;
    router.Connect(Engine::PacketType::ChatMessagePacket, [&](uint32_t, const std::vector<uint8_t>& data) {
        Engine::Deserializer deserializer(data);
        receivedPayload = deserializer.ReadString();
    });

    router.DispatchMessage(1, bytes);
    REQUIRE(receivedPayload == "payload");
}

TEST_CASE("Serializer native endian round trip", "[networking][serializer][endian]")
{
    Engine::Serializer serializer;
    serializer.Write(static_cast<uint16_t>(0x1234))
        .Write(static_cast<uint32_t>(0xAABBCCDD))
        .Write(1.5f);

    const std::vector<uint8_t> bytes = serializer.GetBytes();
    Engine::Deserializer deserializer(bytes, 0);

    REQUIRE(deserializer.ReadUShort() == 0x1234);
    REQUIRE(deserializer.ReadUInt() == 0xAABBCCDDu);
    REQUIRE_THAT(deserializer.ReadFloat(), WithinAbs(1.5f, 0.001f));

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    REQUIRE(bytes.size() >= 2);
    REQUIRE(bytes[0] == 0x34);
    REQUIRE(bytes[1] == 0x12);
#elif defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
    if (std::endian::native == std::endian::little)
    {
        REQUIRE(bytes.size() >= 2);
        REQUIRE(bytes[0] == 0x34);
        REQUIRE(bytes[1] == 0x12);
    }
#endif
}

TEST_CASE("LocalIdentity reads user file", "[networking][localidentity]")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "igneous_local_identity.txt";
    {
        std::ofstream file(path);
        file << "id=12345\n";
        file << "username=Tester\n";
    }

    Engine::LocalIdentity identity(path.string());
    identity.GenerateLocalId();
    identity.GenerateLocalUsername();
    REQUIRE(identity.GetLocalId() == 12345);
    REQUIRE(identity.GetLocalUsername() == "Tester");

    std::filesystem::remove(path);
}

TEST_CASE("LocalNetwork links client and server loopback peers", "[networking][localnetwork]")
{
    Engine::LocalNetwork server;
    Engine::LocalNetwork client;
    server.Connect(true);
    client.Connect(false);

    server.GetLoopback().SetPeer(&client);
    client.GetLoopback().SetPeer(&server);

    REQUIRE(client.Connected());
    REQUIRE(server.Connected());

    client.SendToServer({9, 8, 7}, Engine::TransportType::Reliable);
    server.Clean();
    client.Clean();
}

TEST_CASE("NetworkLoopbackLink forwards to peer callback", "[networking][networkloopbacklink]")
{
    Engine::LocalNetwork network;
    std::vector<Engine::NetworkMessage> received;
    network.onMessageReceived = [&](const Engine::NetworkMessage& message) {
        received.push_back(message);
    };

    Engine::NetworkLoopbackLink link;
    link.SetPeer(&network);

    Engine::NetworkMessage message{
        Engine::NetworkEventType::Message,
        Engine::NetworkPeerIds::Local,
        {1, 2, 3},
    };
    link.Forward(message);

    REQUIRE(link.IsLinked());
    REQUIRE(received.size() == 1);
    REQUIRE(received.front().data == std::vector<uint8_t>{1, 2, 3});
}

TEST_CASE("PacketRouter dispatch and disconnect", "[networking][packetrouter]")
{
    Engine::PacketRouter router;
    int callCount = 0;
    size_t handlerId = router.Connect(Engine::PacketType::ChatMessagePacket, [&](uint32_t, const std::vector<uint8_t>&) {
        ++callCount;
    });

    router.Dispatch(Engine::PacketType::ChatMessagePacket, 1, {0});
    REQUIRE(callCount == 1);

    router.Disconnect(Engine::PacketType::ChatMessagePacket, handlerId);
    router.Dispatch(Engine::PacketType::ChatMessagePacket, 1, {0});
    REQUIRE(callCount == 1);
}

TEST_CASE("ClientConnectionWatchdog timeout and ping", "[networking][clientconnectionwatchdog]")
{
    Engine::ClientConnectionWatchdog watchdog;
    watchdog.enabled = true;
    watchdog.timeoutSeconds = 5.0;
    watchdog.pingIntervalSeconds = 1.0;
    watchdog.Reset(0.0);

    REQUIRE_FALSE(watchdog.HasTimedOut(4.0));
    REQUIRE(watchdog.HasTimedOut(5.0));
    REQUIRE(watchdog.TryConsumePingSend(1.0));
    REQUIRE_FALSE(watchdog.TryConsumePingSend(1.5));
}

TEST_CASE("ServerPeerActivityTracker stale peers", "[networking][serverpeeractivitytracker]")
{
    Engine::ServerPeerActivityTracker tracker;
    tracker.timeoutSeconds = 2.0;
    tracker.TrackPeer(10, 0.0);
    tracker.TrackPeer(20, 0.0);

    tracker.MarkActivity(10, 1.5);
    auto stale = tracker.CollectStalePeerIds(2.5);
    REQUIRE(stale.size() == 1);
    REQUIRE(stale.front() == 20);
}

TEST_CASE("NetworkPeerIds mapping", "[networking][networkpeerids]")
{
    REQUIRE(Engine::NetworkPeerIds::Local == 1);
    REQUIRE(Engine::NetworkPeerIds::FromEnetPeer(0) == 2);
}

TEST_CASE("NetworkSessionFactory creates linked local client server", "[networking][networksessionfactory]")
{
    Engine::NetworkSessionFactory::LocalClientServer session = Engine::NetworkSessionFactory::CreateLocalClientServer();

    REQUIRE(session.server != nullptr);
    REQUIRE(session.client != nullptr);
    REQUIRE(session.server->Connected());
    REQUIRE(session.client->Connected());

    std::vector<uint8_t> payload{4, 5, 6};
    std::vector<uint8_t> received;
    session.server->onMessageReceived = [&](const Engine::NetworkMessage& message) {
        received = message.data;
    };

    session.client->SendToServer(payload, Engine::TransportType::Reliable);
    session.server->Poll();

    REQUIRE(received == payload);

    session.server->Clean();
    session.client->Clean();
}

TEST_CASE("NetworkSessionFactory creates local-only ENet session", "[networking][enet]")
{
    ENetTestFixture enet;
    (void) enet;

    Engine::NetworkSessionFactory::EnetClientServer session = Engine::NetworkSessionFactory::CreateEnetClientServer(true);

    REQUIRE(session.server != nullptr);
    REQUIRE(session.client != nullptr);
    REQUIRE(session.server->Connected());
    REQUIRE(session.client->Connected());

    std::vector<uint8_t> payload{7, 8, 9};
    std::vector<uint8_t> received;
    session.server->onMessageReceived = [&](const Engine::NetworkMessage& message) {
        received = message.data;
    };

    session.client->SendToServer(payload, Engine::TransportType::Reliable);
    session.server->Poll();

    REQUIRE(received == payload);

    session.server->Clean();
    session.client->Clean();
}

TEST_CASE("ENetNetwork does not start UPnP when disabled", "[networking][enet]")
{
    ENetTestFixture enet;
    (void) enet;

    Engine::ENetNetwork server;
    server.Connect(33334, 4, false, false);

    REQUIRE(server.Connected());
    REQUIRE_FALSE(server.IsUpnpActive());

    server.Clean();
}

#ifndef IGNEOUS_STEAM_ENABLED
TEST_CASE("SteamNetwork stub Connect is safe when Steam is disabled", "[networking][steam][stub]")
{
    Engine::SteamNetwork network;
    network.Connect();
    network.Connect(12345);
    REQUIRE_FALSE(network.Connected());

    network.SendToServer({1, 2, 3}, Engine::TransportType::Reliable);
    network.Poll();
    network.Clean();
}
#endif
