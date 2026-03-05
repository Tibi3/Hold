#include "str_table.h"
#include "arena.h"
#include <string.h>


void hold_str_table_init(HoldStrTable* table) {
    table->size = 0;
    table->capacity = 4;
    table->data = calloc(table->capacity, sizeof(*table->data));
    hold_arena_init(&table->arena);
}

void hold_str_table_free(HoldStrTable* table) {
    free((void*)table->data);
    hold_arena_free(&table->arena);
}

const uint8_t* hold_str_hash_table_get_index(HoldStrTable* table, const uint8_t *str, uint64_t size, uint64_t* index_out) {
    uint64_t mask = table->capacity - 1;
    uint64_t hash = fnv1a(str, size);
    uint64_t index = hash & mask;

    for (uint64_t i = 0; i < table->capacity; i++) {
        *index_out = (index + i) & mask;
        HoldStrSlice *slice = &table->data[(index + i) & mask];

        if (slice->str == NULL) {
            return NULL;
        } else {
            if (slice->size == size && slice->str[0] == str[0] && strncmp((char*)str, (char*)slice->str, size) == 0) {
                return slice->str;
            }
        }
    }

    return NULL;
}

const uint8_t* hold_str_table_insert(HoldStrTable* table, const uint8_t *str, uint64_t size) {
    if (table->size / (float)table->capacity >= 0.7) {
        uint32_t old_capacity = table->capacity;
        HoldStrSlice* old_data = table->data;
        table->capacity = table->capacity * 2;
        table->data = calloc(table->capacity, sizeof(*table->data));

        for (uint64_t i = 0; i < old_capacity; i++) {
            if (old_data[i].str == NULL) continue;

            uint64_t index;
            hold_str_hash_table_get_index(table, old_data[i].str, old_data[i].size, &index);
            table->data[index] = old_data[i];
        }

        free(old_data);
    }

    uint64_t index;
    const uint8_t* address = hold_str_hash_table_get_index(table, str, size, &index);

    if (address == NULL) {
        table->data[index].str = hold_arena_alloc(&table->arena, size);
        table->data[index].size = size;
        table->size++;

        strncpy((char*)table->data[index].str, (char*)str, size);

        return table->data[index].str;
    }

    return address;
}

const uint8_t* hold_str_table_get(HoldStrTable* table, const uint8_t *str, uint64_t size) {
    uint64_t index;
    return hold_str_hash_table_get_index(table, str, size, &index);
}

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
uint64_t fnv1a(const uint8_t *str, uint64_t size) {
    uint64_t hash = 14695981039346656037u;

    for (uint64_t i = 0; i < size; i++) {
        hash ^= str[i];
        hash *= 1099511628211;
    }

    return hash;
}
