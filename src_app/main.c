#include "arena.h"
#include "gc.h"
#include "common.h"
#include "lexer.h"
#include "vm.h"
#include "str_table.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // HoldLexer lexer;
    // HoldArena arena;

    // const uint8_t* src = (uint8_t*)"()[]{}<=;<;=;!=";

    // hold_arena_init(&arena);
    // hold_lexer_init(&lexer, &arena, src, (uint8_t*)"main.hold");

    // HoldToken *token;
    // char name[512] = {0};
    // do {
    //     token = hold_lexer_get_next_token(&lexer);
    //     hold_token_to_str(token, name);
    //     printf("%s\n", name);
    // } while(token->kind != HOLD_TOKEN_EOF);

    // return 0;
    HoldVM vm;

    uint32_t instructions[] = {
        // LOAD CONST 0 TO REG0
        0x01000000,
        // LOAD CONST 1 TO REG1
        0x01010001,
        // LOAD CONST 2 TO REG2
        0x01020002,
        // SAVE REG 2 to stack
        0x06000002,
        // SAVE REG 1 to stack
        0x06000101,
        // LOAD REG 0 from stack[0]
        0x02000000,
        // 99 is INVALID OP_CODE
        // 0x99000000,
        // HALT
        0x00000000,
    };

    HoldRegister constants[] = { { .u64 = 0xAA }, { .u64 = 0xBB }, { .u64 = 0xCC } };

    hold_init_vm(&vm, instructions, constants);

    hold_run(&vm);

    printf("reg0: %02lx\n", vm.registers[0].u64);
    printf("reg1: %02lx\n", vm.registers[1].u64);
    printf("reg2: %02lx\n", vm.registers[2].u64);

    printf("stack:\n");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", vm.stack[i]);
    }
    printf("\n");

    hold_free_vm(&vm);

    return 0;
}
