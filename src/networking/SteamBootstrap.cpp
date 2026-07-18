#include "igneous/networking/SteamBootstrap.hpp"

#include <SDL3/SDL_log.h>

#ifdef IGNEOUS_STEAM_ENABLED
#include <steam/steam_api.h>
#endif

namespace Engine
{
#ifdef IGNEOUS_STEAM_ENABLED
    static bool steamInitialized = false;
#endif

    bool SteamBootstrap::Init()
    {
#ifdef IGNEOUS_STEAM_ENABLED
        if (steamInitialized)
            return true;

        if (!SteamAPI_Init())
        {
            SDL_Log("SteamBootstrap: SteamAPI_Init failed. Is Steam running?");
            return false;
        }

        steamInitialized = true;
        SDL_Log("SteamBootstrap: Steam initialized.");
        return true;
#else
        SDL_Log("SteamBootstrap: Steamworks not compiled in (DIGNEOUS_STEAM=OFF).");
        return false;
#endif
    }

    void SteamBootstrap::RunCallbacks()
    {
#ifdef IGNEOUS_STEAM_ENABLED
        if (steamInitialized)
            SteamAPI_RunCallbacks();
#endif
    }

    void SteamBootstrap::Shutdown()
    {
#ifdef IGNEOUS_STEAM_ENABLED
        if (!steamInitialized)
            return;

        SteamAPI_Shutdown();
        steamInitialized = false;
#endif
    }

    bool SteamBootstrap::IsInitialized()
    {
#ifdef IGNEOUS_STEAM_ENABLED
        return steamInitialized;
#else
        return false;
#endif
    }
}
