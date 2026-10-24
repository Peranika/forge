#include "forge/game.h"
#include "forge/log.h"
#include "forge/mem.h"
#include <string.h>

typedef struct BuildIdNote {
    u32 name_size;
    u32 desc_size;
    u32 type;
    char name[4];
    u8 desc[];
} BuildIdNote;

static const ForgeGame s_games[] = {
    {
        .id = ForgeGameId_MHGU,
        .name = "MHGU",
        .build_id = { 0xFB, 0x08, 0xF1, 0xD2, 0x0F, 0xD1, 0x20, 0x4F, 0xE7, 0x07, 0x06, 0x72, 0x77, 0x7F, 0x86, 0x9B },
        .app_run = 0xB8692C,
        .update = 0x3DA8FC,
        .system_ctor = 0x887810,
        .system_dtor = 0x88784C,
        .mount_save_data = 0x878DA0,
        .job_safe = 0x212041C,
        .socket_init = 0x813B44,
        .socket_object = 0x191EEE8,
        .dti_find = 0x7AEF68,
        .random_instance = 0x191EBB8,
        .random_nrand = 0x7CC110,
    },
    {
        // MHXX (JP) 1.5.1
        .id = ForgeGameId_MHXX,
        .name = "MHXX",
        .build_id = { 0x9D, 0x4C, 0x86, 0xE6, 0xEF, 0x74, 0x50, 0x4A, 0xFA, 0x3A, 0x48, 0x5C, 0x97, 0x97, 0x65, 0x3B },
        .app_run = 0xB7B344,
        .update = 0x3D6BE0,
        .system_ctor = 0x87C22C,
        .system_dtor = 0x87C268,
        .mount_save_data = 0x86D7BC,
        .job_safe = 0x214241C,
        .socket_init = 0x8085C4,
        .socket_object = 0x1940188,
        .dti_find = 0x7A39E8,
        .random_instance = 0x193FE58,
        .random_nrand = 0x7C0B90,
    },
};

const ForgeGame* g_game = NULL;

// The GNU build ID note sits near the end of main's rodata.
static const u8* findBuildId(void)
{
    MemoryInfo info;
    if (R_FAILED(forge_mem_queryMemory(&info, g_mainRodataAddr))) {
        return NULL;
    }

    const u32 begin = info.addr;
    const u32 end = info.addr + info.size;

    for (u32 addr = end - sizeof(BuildIdNote) - 0x10; addr >= begin; addr -= 4) {
        const BuildIdNote* note = (const BuildIdNote*)addr;
        if (note->name_size == 4 && note->desc_size >= 0x10 && note->type == 3 && memcmp(note->name, "GNU", 4) == 0) {
            return note->desc;
        }
    }

    return NULL;
}

bool forge_game_init(void)
{
    const u8* build_id = findBuildId();
    if (build_id == NULL) {
        forge_log_trace("[forge] Could not find the build ID of main");
        return false;
    }

    for (size_t i = 0; i < sizeof(s_games) / sizeof(s_games[0]); i++) {
        if (memcmp(s_games[i].build_id, build_id, sizeof(s_games[i].build_id)) == 0) {
            g_game = &s_games[i];
            forge_log_trace("[forge] Detected %s", g_game->name);
            return true;
        }
    }

    forge_log_trace("[forge] Unsupported game version, build ID %02X%02X%02X%02X%02X%02X%02X%02X...",
        build_id[0], build_id[1], build_id[2], build_id[3], build_id[4], build_id[5], build_id[6], build_id[7]);
    return false;
}

ForgeGameId forge_game_getId(void)
{
    return g_game != NULL ? g_game->id : ForgeGameId_Unknown;
}

const char* forge_game_getName(void)
{
    return g_game != NULL ? g_game->name : "Unknown";
}
