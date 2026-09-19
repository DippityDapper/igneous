#pragma once

namespace Engine
{

    class SteamBootstrap
    {
      public:
        static bool Init();

        static void RunCallbacks();

        static void Shutdown();

        static bool IsInitialized();
    };
}
