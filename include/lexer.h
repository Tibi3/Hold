#pragma once

#include "arena.h"
#include "common.h"
#include <stdint.h>

typedef enum HoldTokenKind {
    HOLD_TOKEN_EOF,
    HOLD_TOKEN_ERROR,

    HOLD_TOKEN_SEMI,
    HOLD_TOKEN_COLON,
    HOLD_TOKEN_DOUBLE_COLON,
    HOLD_TOKEN_COMMA,

    HOLD_TOKEN_LEFT_PAREN,
    HOLD_TOKEN_RIGHT_PAREN,
    HOLD_TOKEN_LEFT_BRACKET,
    HOLD_TOKEN_RIGHT_BRACKT,
    HOLD_TOKEN_LEFT_BRACE,
    HOLD_TOKEN_RIGHT_BRACE,

    HOLD_TOKEN_EQ,
    HOLD_TOKEN_NOT_EQ,
    HOLD_TOKEN_LESS_THAN,
    HOLD_TOKEN_LESS_THAN_EQ,
    HOLD_TOKEN_GREATER_THAN,
    HOLD_TOKEN_GREATER_THAN_EQ,
    HOLD_TOKEN_ASSIGN,
    HOLD_TOKEN_BANG,

    HOLD_TOKEN_STRUCT,
    HOLD_TOKEN_TRAIT,
    HOLD_TOKEN_FN,
    HOLD_TOKEN_IF,
    HOLD_TOKEN_WHILE,
    HOLD_TOKEN_RETURN,

    HOLD_TOKEN_LITERAL_I64,
    HOLD_TOKEN_LITERAL_F64,
    HOLD_TOKEN_LITERAL_STR,
    HOLD_TOKEN_LITERAL_BOOL,
    HOLD_TOKEN_IDENTIFIER,
} HoldTokenKind;

typedef struct HoldToken {
    HoldTokenKind kind;
    const uint8_t* position;
    union {
        uint8_t* str;
        uint64_t i64;
        double f64;
    };
} HoldToken;

typedef struct HoldLexer {
    const uint8_t *cursor;
    const uint8_t *current_file_path;

    uint32_t current_char;
    uint32_t next_char;

    uint32_t line;
    uint32_t column;

    HoldArena *token_arena;
} HoldLexer;

void hold_token_to_str(HoldToken* token, char *src);

void hold_lexer_init(HoldLexer *lexer, HoldArena *token_arena, const uint8_t *src, const uint8_t *file_path);
void hold_lexer_free(HoldLexer *lexer);

HoldToken* hold_lexer_get_next_token(HoldLexer *lexer);
