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
    HoldLexer lexer;
    HoldArena arena;

    const uint8_t* src = (uint8_t*)"()[]{}<=;<;=;!=";

    hold_arena_init(&arena);
    hold_lexer_init(&lexer, &arena, src, (uint8_t*)"main.hold");

    HoldToken *token;
    char name[512] = {0};
    do {
        token = hold_lexer_get_next_token(&lexer);
        hold_token_to_str(token, name);
        printf("%s\n", name);
    } while(token->kind != HOLD_TOKEN_EOF);

    return 0;
    HoldVM vm;

    uint32_t instructions[] = {
        // ALLOC 10 bytes (+ header)
        0x1200000A,
        // ALLOC 10 bytes (+ header)
        0x1200000A,
        // ALLOC 10 bytes (+ header)
        0x1200000A,
        // ALLOC 10 bytes (+ header)
        0x1200000A,
        // LOAD CONST
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
        0x01000000,
    };

    hold_init_vm(&vm, instructions);

    vm.constants[0].f64 = 0.25;
    vm.constants[1].f64 = 0.26;
    vm.constants[2].f64 = 0.27;

    vm.running = true;
    for (int i = 0; i < 4; i++) {
        if (!vm.running) break;

        hold_tick(&vm);
        printf("objects %ld\n", vm.gc.tracked_object_count);
    }

    printf("---\n");

    hold_gc_mark_and_sweep(&vm.gc, &vm);
    printf("objects %ld\n", vm.gc.tracked_object_count);

    printf("---\n");

    vm.osp = vm.frame_stack[0].osp + 2;
    hold_gc_mark_and_sweep(&vm.gc, &vm);
    printf("objects %ld\n", vm.gc.tracked_object_count);

    hold_free_vm(&vm);

    return 0;
}
