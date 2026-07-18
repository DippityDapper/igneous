#include <catch2/catch_test_macros.hpp>

#include "igneous/networking/ClientConnectionWatchdog.hpp"
#include "igneous/networking/LocalIdentity.hpp"
#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkLoopbackLink.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"
#include "igneous/networking/PacketRouter.hpp"
#include "igneous/networking/PacketTypes.hpp"
#include "igneous/networking/Serializer.hpp"
#include "igneous/networking/ServerPeerActivityTracker.hpp"

#include <filesystem>
#include <fstream>

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
