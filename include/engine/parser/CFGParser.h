#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdlib>

#include "SDL3/SDL.h"

namespace Engine
{
    class CFGParser
    {
    private:
        static std::map<std::string, std::map<std::string, std::string>> configs;

    public:
        static void LoadConfig(const char* fileName);

        static const char* GetString(const char* configName, const char* key);
        static int GetInt(const char* configName, const char* key);
        static int GetBool(const char* configName, const char* key);
    };
}
