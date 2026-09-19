#pragma once

#include <cmath>
#include <cstdint>
#include <string>

struct SDL_Surface;

namespace Engine
{

    class Perlin
    {
      public:

        int octaves = 6;
        float scale = 0.005f;
        float lacunarity = 2.0f;
        float gain = 0.5f;
        int width = 512;
        int height = 512;

        Perlin()
        {
            RebuildTable();
        }

        explicit Perlin(uint32_t seed_)
            : seed(seed_)
        {
            RebuildTable();
        }

        void SetSeed(uint32_t seed_)
        {
            if (seed == seed_)
                return;

            seed = seed_;
            RebuildTable();
        }

        uint32_t GetSeed() const
        {
            return seed;
        }

        void RebuildTable();

        float Noise2(float x, float y) const
        {
            int xi = static_cast<int>(std::floor(x)) & 255;
            int yi = static_cast<int>(std::floor(y)) & 255;

            float xf = x - std::floor(x);
            float yf = y - std::floor(y);
            float u = Fade(xf);
            float v = Fade(yf);

            uint8_t aa = perm[perm[xi] + yi];
            uint8_t ab = perm[perm[xi] + yi + 1];
            uint8_t ba = perm[perm[xi + 1] + yi];
            uint8_t bb = perm[perm[xi + 1] + yi + 1];

            float x1 = Lerp(Grad2(aa, xf, yf), Grad2(ba, xf - 1.0f, yf), u);
            float x2 = Lerp(Grad2(ab, xf, yf - 1.0f), Grad2(bb, xf - 1.0f, yf - 1.0f), u);
            return Lerp(x1, x2, v);
        }

        float Noise3(float x, float y, float z) const
        {
            int xi = static_cast<int>(std::floor(x)) & 255;
            int yi = static_cast<int>(std::floor(y)) & 255;
            int zi = static_cast<int>(std::floor(z)) & 255;

            float xf = x - std::floor(x);
            float yf = y - std::floor(y);
            float zf = z - std::floor(z);
            float u = Fade(xf);
            float v = Fade(yf);
            float w = Fade(zf);

            uint8_t aaa = perm[perm[perm[xi] + yi] + zi];
            uint8_t aab = perm[perm[perm[xi] + yi] + zi + 1];
            uint8_t aba = perm[perm[perm[xi] + yi + 1] + zi];
            uint8_t abb = perm[perm[perm[xi] + yi + 1] + zi + 1];
            uint8_t baa = perm[perm[perm[xi + 1] + yi] + zi];
            uint8_t bab = perm[perm[perm[xi + 1] + yi] + zi + 1];
            uint8_t bba = perm[perm[perm[xi + 1] + yi + 1] + zi];
            uint8_t bbb = perm[perm[perm[xi + 1] + yi + 1] + zi + 1];

            float x1 = Lerp(Grad3(aaa, xf, yf, zf), Grad3(baa, xf - 1.0f, yf, zf), u);
            float x2 = Lerp(Grad3(aba, xf, yf - 1.0f, zf), Grad3(bba, xf - 1.0f, yf - 1.0f, zf), u);
            float x3 = Lerp(Grad3(aab, xf, yf, zf - 1.0f), Grad3(bab, xf - 1.0f, yf, zf - 1.0f), u);
            float x4 = Lerp(Grad3(abb, xf, yf - 1.0f, zf - 1.0f), Grad3(bbb, xf - 1.0f, yf - 1.0f, zf - 1.0f), u);

            float y1 = Lerp(x1, x2, v);
            float y2 = Lerp(x3, x4, v);
            return Lerp(y1, y2, w);
        }

        float Fbm2(float x, float y) const
        {
            float value = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxValue = 0.0f;

            for (int i = 0; i < octaves; i++)
            {
                value += Noise2(x * scale * frequency, y * scale * frequency) * amplitude;
                maxValue += amplitude;
                amplitude *= gain;
                frequency *= lacunarity;
            }

            return value / maxValue;
        }

        float Fbm3(float x, float y, float z) const
        {
            float value = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxValue = 0.0f;

            for (int i = 0; i < octaves; i++)
            {
                value += Noise3(x * scale * frequency, y * scale * frequency, z * scale * frequency) * amplitude;
                maxValue += amplitude;
                amplitude *= gain;
                frequency *= lacunarity;
            }

            return value / maxValue;
        }

        float Fbm2_01(float x, float y) const
        {
            return (Fbm2(x, y) + 1.0f) * 0.5f;
        }

        float Fbm3_01(float x, float y, float z) const
        {
            return (Fbm3(x, y, z) + 1.0f) * 0.5f;
        }

        bool SavePng(const std::string& path) const;

        bool SaveData(const std::string& path) const;

        SDL_Surface* LoadData(const std::string& path);

      private:

        uint32_t seed = 0;
        uint8_t perm[512];

        SDL_Surface* BuildSurface() const;

        static float Fade(float t)
        {
            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }
        static float Lerp(float a, float b, float t)
        {
            return a + t * (b - a);
        }

        static float Grad2(uint8_t hash, float x, float y)
        {
            switch (hash & 7)
            {
            case 0:
                return x + y;
            case 1:
                return -x + y;
            case 2:
                return x - y;
            case 3:
                return -x - y;
            case 4:
                return x;
            case 5:
                return -x;
            case 6:
                return y;
            case 7:
                return -y;
            default:
                return 0.0f;
            }
        }

        static float Grad3(uint8_t hash, float x, float y, float z)
        {
            switch (hash & 15)
            {
            case 0:
                return x + y;
            case 1:
                return -x + y;
            case 2:
                return x - y;
            case 3:
                return -x - y;
            case 4:
                return x + z;
            case 5:
                return -x + z;
            case 6:
                return x - z;
            case 7:
                return -x - z;
            case 8:
                return y + z;
            case 9:
                return -y + z;
            case 10:
                return y - z;
            case 11:
                return -y - z;
            case 12:
                return x + y;
            case 13:
                return -y + z;
            case 14:
                return -x + y;
            case 15:
                return -y - z;
            default:
                return 0.0f;
            }
        }
    };

}
