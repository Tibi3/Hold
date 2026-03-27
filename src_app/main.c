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

#define INSTRUCTION(instruction, value) ((instruction) << 24 | (value))

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
        INSTRUCTION(HOLD_INSTRUCTION_LOAD_CONSTANT, 0x0),
        // LOAD CONST 1 TO REG1
        INSTRUCTION(HOLD_INSTRUCTION_LOAD_CONSTANT, 0x00010001),
        // LOAD CONST 2 TO REG2
        INSTRUCTION(HOLD_INSTRUCTION_LOAD_CONSTANT, 0x00020002),
        // LOAD CONST 3 TO REG3
        INSTRUCTION(HOLD_INSTRUCTION_LOAD_CONSTANT, 0x00030003),
        // SAVE REG 2 to stack
        INSTRUCTION(HOLD_INSTRUCTION_SAVE_U8_TO_STACK, 0x00000002),
        // SAVE REG 1 to stack
        INSTRUCTION(HOLD_INSTRUCTION_SAVE_U8_TO_STACK, 0x00000101),
        // LOAD REG 4 from stack[0]
        INSTRUCTION(HOLD_INSTRUCTION_LOAD_U8_FROM_STACK, 0x00040000),
        // SAVE REG 3 to stack [4..=7]
        INSTRUCTION(HOLD_INSTRUCTION_SAVE_U32_TO_STACK, 0x00000403),
        // 99 is INVALID OP_CODE
        // 0x99000000,
        // HALT
        INSTRUCTION(HOLD_INSTRUCTION_HALT, 0x0)
    };


    HoldRegister constants[] = { { .u64 = 0xAA }, { .u64 = 0xBB }, { .u64 = 0xCC }, { .f32 = 0.111 } };

    hold_init_vm(&vm, instructions, constants);

    hold_run(&vm);

    printf("reg0: %02lx\n", vm.registers[0].u64);
    printf("reg1: %02lx\n", vm.registers[1].u64);
    printf("reg2: %02lx\n", vm.registers[2].u64);
    printf("reg3: %f\n", vm.registers[3].f32);
    printf("reg4: %02lx\n", vm.registers[4].u64);

    printf("stack:\n");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", vm.stack[i]);
    }
    printf("\n");

    hold_free_vm(&vm);

    return 0;
}
