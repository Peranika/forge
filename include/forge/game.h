#pragma once

#include "switch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ForgeGameId {
    ForgeGameId_Unknown = 0,
    ForgeGameId_MHGU,
    ForgeGameId_MHXX,
} ForgeGameId;

// Offsets are relative to g_mainTextAddr.
typedef struct ForgeGame {
    ForgeGameId id;
    const char* name;
    u8 build_id[0x10];

    u32 app_run;
    u32 update;
    u32 system_ctor;
    u32 system_dtor;
    u32 mount_save_data;
    u32 job_safe;
    u32 socket_init;
    u32 socket_object;
    u32 dti_find;
    u32 random_instance;
    u32 random_nrand;
} ForgeGame;

extern const ForgeGame* g_game;

// Identifies the running game by the build ID of main. Must be called after forge_mem_init.
bool forge_game_init(void);

ForgeGameId forge_game_getId(void);
const char* forge_game_getName(void);

#ifdef __cplusplus
}
#endif
