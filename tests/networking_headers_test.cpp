#include <catch2/catch_test_macros.hpp>

#include "igneous/networking/IdentityProvider.hpp"

TEST_CASE("IdentityProvider.hpp is self-contained", "[networking][headers]")
{
    struct TestIdentityProvider : Engine::IdentityProvider
    {
        uint64_t GetLocalId() override { return 0; }
        std::string GetLocalUsername() override { return "test"; }
        void GenerateLocalId() override {}
        void GenerateLocalUsername() override {}
        std::vector<uint8_t> GetAuthToken(uint64_t) override { return {}; }
        bool ValidateToken(uint64_t, const std::vector<uint8_t>&) override { return false; }
        void OnDisconnect(uint64_t) override {}
        void Clean() override {}
    };

    TestIdentityProvider provider;
    REQUIRE(provider.GetLocalUsername() == "test");
}
