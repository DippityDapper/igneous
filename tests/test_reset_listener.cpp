#include "TestReset.hpp"

#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdlib>

namespace
{
    struct HeadlessSdlVideoDriver
    {
        HeadlessSdlVideoDriver()
        {
            setenv("SDL_VIDEODRIVER", "dummy", 0);
        }
    };

    const HeadlessSdlVideoDriver installHeadlessSdlVideoDriver{};

    class TestResetListener : public Catch::EventListenerBase
    {
      public:
        using Catch::EventListenerBase::EventListenerBase;

        void testCaseStarting(Catch::TestCaseInfo const&) override
        {
            Engine::TestReset::All();
        }

        void testCaseEnded(Catch::TestCaseStats const&) override
        {
            Engine::TestReset::All();
        }
    };
}

CATCH_REGISTER_LISTENER(TestResetListener)
