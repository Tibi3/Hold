#include "lexer.h"
#include "arena.h"
#include <stdint.h>
#include <stdio.h>

static void next(HoldLexer *lexer);
static HoldToken* create_token_based_on_next_char(HoldLexer *lexer, uint8_t charater, HoldTokenKind kind1, HoldTokenKind kind2);
static HoldToken* create_token_f64(HoldLexer *lexer, HoldTokenKind kind, double value);
static HoldToken* create_token_i64(HoldLexer *lexer, HoldTokenKind kind, uint64_t value);
static HoldToken* create_token_str(HoldLexer *lexer, HoldTokenKind kind, uint8_t* value);
static HoldToken* create_token_empty(HoldLexer *lexer, HoldTokenKind kind);


void hold_lexer_init(HoldLexer *lexer, HoldArena *token_arena, const uint8_t *src, const uint8_t *file_path) {
    lexer->cursor = src;
    lexer->current_file_path = file_path;
    lexer->token_arena = token_arena;
    lexer->column = 0;
    lexer->line = 0;
    lexer->current_char = lexer->next_char = ' ';

    next(lexer);
}

void hold_lexer_free(HoldLexer *lexer) {}

HoldToken* hold_lexer_get_next_token(HoldLexer *lexer) {
    next(lexer);

    switch (lexer->current_char) {
        case '\0':  return create_token_empty(lexer, HOLD_TOKEN_EOF);
        case ';':   return create_token_empty(lexer, HOLD_TOKEN_SEMI);
        case ',':   return create_token_empty(lexer, HOLD_TOKEN_COMMA);
        case '(':   return create_token_empty(lexer, HOLD_TOKEN_LEFT_PAREN);
        case ')':   return create_token_empty(lexer, HOLD_TOKEN_RIGHT_PAREN);
        case '[':   return create_token_empty(lexer, HOLD_TOKEN_LEFT_BRACKET);
        case ']':   return create_token_empty(lexer, HOLD_TOKEN_RIGHT_BRACKT);
        case '{':   return create_token_empty(lexer, HOLD_TOKEN_LEFT_BRACE);
        case '}':   return create_token_empty(lexer, HOLD_TOKEN_RIGHT_BRACE);
        case '<':   return create_token_based_on_next_char(lexer, '=', HOLD_TOKEN_LESS_THAN, HOLD_TOKEN_LESS_THAN_EQ);
        case '>':   return create_token_based_on_next_char(lexer, '=', HOLD_TOKEN_GREATER_THAN, HOLD_TOKEN_GREATER_THAN_EQ);
        case '=':   return create_token_based_on_next_char(lexer, '=', HOLD_TOKEN_ASSIGN, HOLD_TOKEN_EQ);
        case '!':   return create_token_based_on_next_char(lexer, '=', HOLD_TOKEN_BANG, HOLD_TOKEN_NOT_EQ);
        case ':':   return create_token_based_on_next_char(lexer, ':', HOLD_TOKEN_COLON, HOLD_TOKEN_DOUBLE_COLON);
    }

    return create_token_empty(lexer, HOLD_TOKEN_ERROR);
};

static void next(HoldLexer *lexer) {
    uint32_t character = 0;
    do {
        uint32_t byte = *(lexer->cursor++);
        if (byte >= 0b11110000) {
            character = byte << 24 | *(lexer->cursor++) << 16 | *(lexer->cursor++) << 8 | *(lexer->cursor++);
        } else if (byte >= 0b11100000) {
            character = byte << 16 | *(lexer->cursor++) << 8 | *(lexer->cursor++) << 0;
        } else if (byte >= 0b11000000) {
            character = byte << 8 | *(lexer->cursor++) << 0;
        } else {
            character = byte;
        }
        lexer->column++;
    } while(character == '\r');

    lexer->current_char = lexer->next_char;
    lexer->next_char = character;

    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 0;
    }
}

static HoldToken* create_token_based_on_next_char(HoldLexer *lexer, uint8_t charater, HoldTokenKind kind1, HoldTokenKind kind2) {
    if (lexer->next_char == charater) {
        next(lexer);
        return create_token_empty(lexer, kind2);
    }

    return create_token_empty(lexer, kind1);
}

static HoldToken* create_token_f64(HoldLexer *lexer, HoldTokenKind kind, double value) {
    HoldToken* token = create_token_empty(lexer, kind);
    token->f64 = value;
    return token;
}

static HoldToken* create_token_i64(HoldLexer *lexer, HoldTokenKind kind, uint64_t value) {
    HoldToken* token = create_token_empty(lexer, kind);
    token->i64 = value;
    return token;
}

static HoldToken* create_token_str(HoldLexer *lexer, HoldTokenKind kind, uint8_t* value) {
    HoldToken* token = create_token_empty(lexer, kind);
    token->str = value;
    return token;
}

static HoldToken* create_token_empty(HoldLexer *lexer, HoldTokenKind kind) {
    HoldToken* token = hold_arena_alloc(lexer->token_arena, sizeof(HoldToken));
    token->kind = kind;
    token->position = lexer->cursor;
    return token;
}

void hold_token_to_str(HoldToken* token, char *src) {
    switch (token->kind) {
        case HOLD_TOKEN_EOF:
            sprintf(src, "HOLD_TOKEN_EOF");
            return;
        case HOLD_TOKEN_ERROR:
            sprintf(src, "HOLD_TOKEN_ERROR");
            return;
        case HOLD_TOKEN_SEMI:
            sprintf(src, "HOLD_TOKEN_SEMI");
            return;
        case HOLD_TOKEN_COLON:
            sprintf(src, "HOLD_TOKEN_COLON");
            return;
        case HOLD_TOKEN_DOUBLE_COLON:
            sprintf(src, "HOLD_TOKEN_DOUBLE_COLON");
            return;
        case HOLD_TOKEN_COMMA:
            sprintf(src, "HOLD_TOKEN_COMMA");
            return;
        case HOLD_TOKEN_LEFT_PAREN:
            sprintf(src, "HOLD_TOKEN_LEFT_PAREN");
            return;
        case HOLD_TOKEN_RIGHT_PAREN:
            sprintf(src, "HOLD_TOKEN_RIGHT_PAREN");
            return;
        case HOLD_TOKEN_LEFT_BRACKET:
            sprintf(src, "HOLD_TOKEN_LEFT_BRACKET");
            return;
        case HOLD_TOKEN_RIGHT_BRACKT:
            sprintf(src, "HOLD_TOKEN_RIGHT_BRACKT");
            return;
        case HOLD_TOKEN_LEFT_BRACE:
            sprintf(src, "HOLD_TOKEN_LEFT_BRACE");
            return;
        case HOLD_TOKEN_RIGHT_BRACE:
            sprintf(src, "HOLD_TOKEN_RIGHT_BRACE");
            return;
        case HOLD_TOKEN_EQ:
            sprintf(src, "HOLD_TOKEN_EQ");
            return;
        case HOLD_TOKEN_NOT_EQ:
            sprintf(src, "HOLD_TOKEN_NOT_EQ");
            return;
        case HOLD_TOKEN_LESS_THAN:
            sprintf(src, "HOLD_TOKEN_LESS_THAN");
            return;
        case HOLD_TOKEN_LESS_THAN_EQ:
            sprintf(src, "HOLD_TOKEN_LESS_THAN_EQ");
            return;
        case HOLD_TOKEN_GREATER_THAN:
            sprintf(src, "HOLD_TOKEN_GREATER_THAN");
            return;
        case HOLD_TOKEN_GREATER_THAN_EQ:
            sprintf(src, "HOLD_TOKEN_GREATER_THAN_EQ");
            return;
        case HOLD_TOKEN_ASSIGN:
            sprintf(src, "HOLD_TOKEN_ASSIGN");
            return;
        case HOLD_TOKEN_BANG:
            sprintf(src, "HOLD_TOKEN_BANG");
            return;
        case HOLD_TOKEN_STRUCT:
            sprintf(src, "HOLD_TOKEN_STRUCT");
            return;
        case HOLD_TOKEN_TRAIT:
            sprintf(src, "HOLD_TOKEN_TRAIT");
            return;
        case HOLD_TOKEN_FN:
            sprintf(src, "HOLD_TOKEN_FN");
            return;
        case HOLD_TOKEN_IF:
            sprintf(src, "HOLD_TOKEN_IF");
            return;
        case HOLD_TOKEN_WHILE:
            sprintf(src, "HOLD_TOKEN_WHILE");
            return;
        case HOLD_TOKEN_RETURN:
            sprintf(src, "HOLD_TOKEN_RETURN");
            return;
        case HOLD_TOKEN_LITERAL_I64:
            sprintf(src, "HOLD_TOKEN_LITERAL_I64");
            return;
        case HOLD_TOKEN_LITERAL_F64:
            sprintf(src, "HOLD_TOKEN_LITERAL_F64");
            return;
        case HOLD_TOKEN_LITERAL_STR:
            sprintf(src, "HOLD_TOKEN_LITERAL_STR");
            return;
        case HOLD_TOKEN_LITERAL_BOOL:
            sprintf(src, "HOLD_TOKEN_LITERAL_BOOL");
            return;
        case HOLD_TOKEN_IDENTIFIER:
            sprintf(src, "HOLD_TOKEN_IDENTIFIER");
            return;
        default:
            sprintf(src, "UnknownToken");
    }

}
