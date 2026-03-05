#pragma once

#include "arena.h"
#include "common.h"

typedef struct HoldStrTable {
    HoldStrSlice *data;
    HoldArena arena;
    uint32_t size;
    uint32_t capacity;
} HoldStrTable;

void hold_str_table_init(HoldStrTable* table);
void hold_str_table_free(HoldStrTable* table);

const uint8_t* hold_str_table_get(HoldStrTable* table, const uint8_t *str, uint64_t size);
const uint8_t* hold_str_table_insert(HoldStrTable* table, const uint8_t *str, uint64_t size);

uint64_t fnv1a(const uint8_t *str, uint64_t size);
