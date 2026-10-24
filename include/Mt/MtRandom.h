#pragma once
#ifdef __cplusplus

#include "MtFunc.h"
#include "forge/game.h"
#include "forge/mem.h"

struct MtRandom {
    u32 x, y, z, w;

    static inline u32 nrand()
    {
        const auto instance = (MtRandom*)(g_mainTextAddr + g_game->random_instance);
        return nrand(*instance);
    }

    static inline u32 nrand(MtRandom& random)
    {
        return MtFunc::invoke<u32>(g_game->random_nrand, &random);
    }
};

#endif
