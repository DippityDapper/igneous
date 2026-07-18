#include "igneous/engine/PerlinNoise.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cstdio>
#include <cstring>

namespace Engine
{

    void Perlin::RebuildTable()
    {
        for (int i = 0; i < 256; i++)
            perm[i] = static_cast<uint8_t>(i);

        uint32_t s = seed;
        for (int i = 255; i > 0; i--)
        {
            s = s * 1664525u + 1013904223u;
            int j = static_cast<int>(s >> 24) % (i + 1);
            uint8_t tmp = perm[i];
            perm[i] = perm[j];
            perm[j] = tmp;
        }

        for (int i = 0; i < 256; i++)
            perm[256 + i] = perm[i];
    }

    SDL_Surface* Perlin::BuildSurface() const
    {
        SDL_Surface* surf = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
        if (!surf)
        {
            SDL_Log("Perlin::BuildSurface: SDL_CreateSurface failed: %s", SDL_GetError());
            return nullptr;
        }

        auto* pixels = static_cast<uint8_t*>(surf->pixels);
        int pitch = surf->pitch;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float n = Fbm2_01(static_cast<float>(x), static_cast<float>(y));
                uint8_t grey = static_cast<uint8_t>(n * 255.0f);

                uint8_t* px = pixels + y * pitch + x * 4;
                px[0] = grey;
                px[1] = grey;
                px[2] = grey;
                px[3] = 255;
            }
        }

        return surf;
    }

    bool Perlin::SavePng(const std::string& path) const
    {
        SDL_Surface* surf = BuildSurface();
        if (!surf)
            return false;

        bool ok = IMG_SavePNG(surf, path.c_str());
        if (!ok)
            SDL_Log("Perlin::SavePng: IMG_SavePNG failed: %s", SDL_GetError());

        SDL_DestroySurface(surf);
        return ok;
    }

    bool Perlin::SaveData(const std::string& path) const
    {
        std::FILE* f = std::fopen(path.c_str(), "w");
        if (!f)
        {
            SDL_Log("Perlin::SaveData: cannot open '%s' for writing", path.c_str());
            return false;
        }

        std::fprintf(f, "seed       %u\n", seed);
        std::fprintf(f, "octaves    %d\n", octaves);
        std::fprintf(f, "scale      %f\n", scale);
        std::fprintf(f, "lacunarity %f\n", lacunarity);
        std::fprintf(f, "gain       %f\n", gain);
        std::fprintf(f, "width      %d\n", width);
        std::fprintf(f, "height     %d\n", height);

        std::fclose(f);
        return true;
    }

    SDL_Surface* Perlin::LoadData(const std::string& path)
    {
        std::FILE* f = std::fopen(path.c_str(), "r");
        if (!f)
        {
            SDL_Log("Perlin::LoadData: cannot open '%s' for reading", path.c_str());
            return nullptr;
        }

        char key[64];
        while (std::fscanf(f, "%63s", key) == 1)
        {
            if (std::strcmp(key, "seed") == 0)
                std::fscanf(f, "%u", &seed);
            else if (std::strcmp(key, "octaves") == 0)
                std::fscanf(f, "%d", &octaves);
            else if (std::strcmp(key, "scale") == 0)
                std::fscanf(f, "%f", &scale);
            else if (std::strcmp(key, "lacunarity") == 0)
                std::fscanf(f, "%f", &lacunarity);
            else if (std::strcmp(key, "gain") == 0)
                std::fscanf(f, "%f", &gain);
            else if (std::strcmp(key, "width") == 0)
                std::fscanf(f, "%d", &width);
            else if (std::strcmp(key, "height") == 0)
                std::fscanf(f, "%d", &height);
        }

        std::fclose(f);

        RebuildTable();
        return BuildSurface();
    }

}
