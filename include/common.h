#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HOLD_ALIGN_MEMORY(address, to) ((((uintptr_t)address) + (to - 1)) & ~(to - 1));

typedef struct HoldStrSlice {
    const uint8_t *str;
    uint64_t size;
} HoldStrSlice;
