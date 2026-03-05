#pragma once

#include "common.h"
#include <stdint.h>

typedef struct Lexer {
    uint8_t *cursor;
    uint32_t current_char;
    uint32_t next_char;
} Lexer;
