#include "arena.h"
#include "common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define ARENA_BLOCK_SIZE 1024

HoldArenaBlock* hold_arena_block_create();

void hold_arena_init(HoldArena* arena) {
    arena->first = hold_arena_block_create();
    arena->current = arena->first;
}

void hold_arena_free(HoldArena* arena) {
    HoldArenaBlock* block = arena->first;

    while(block != NULL) {
        HoldArenaBlock* next = block->next;
        free(block);
        block = next;
    }
}

void* hold_arena_alloc(HoldArena* arena, uint64_t bytes) {
    void* address = &arena->current->data[arena->current->size];
    void* aligned_address = (void*)HOLD_ALIGN_MEMORY(address, 8)
    uint64_t aligned_offset = aligned_address - address;

    if (arena->current->capacity - (arena->current->size + aligned_offset) < bytes) {
        arena->current->next = hold_arena_block_create();
        arena->current = arena->current->next;

        aligned_address = &arena->current->data[0];
        aligned_offset = 0;
    }

    arena->current->size += bytes + aligned_offset;

    return aligned_address;
}

HoldArenaBlock* hold_arena_block_create() {
    HoldArenaBlock* block = malloc(sizeof(HoldArenaBlock) + ARENA_BLOCK_SIZE);

    block->size = 0;
    block->capacity = ARENA_BLOCK_SIZE;
    block->next = NULL;

    return block;
}
