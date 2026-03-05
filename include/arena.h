#pragma once

#include "common.h"

typedef struct HoldArenaBlock {
    struct HoldArenaBlock *next;
    uint64_t size;
    uint64_t capacity;
    uint8_t data[];
} HoldArenaBlock;

typedef struct HoldArena {
    HoldArenaBlock *first;
    HoldArenaBlock *current;
} HoldArena;

void hold_arena_init(HoldArena* arena);
void hold_arena_free(HoldArena* arena);

void* hold_arena_alloc(HoldArena* arena, uint64_t bytes);
