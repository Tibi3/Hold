#include "vm.h"
#include "gc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hold_init_vm(HoldVM *vm, const uint32_t *code, const HoldRegister* constants) {
    vm->code = code;
    vm->constants = constants;
    vm->pc = vm->code;
    vm->sp = vm->stack;
    vm->sp_base = vm->sp;
    vm->osp = vm->object_stack;
    vm->osp_base = vm->osp;
    vm->reg_base = vm->registers;

    hold_gc_init(&vm->gc);
}

void hold_free_vm(HoldVM *vm) {
    hold_gc_free(&vm->gc);
}

void hold_run(HoldVM *vm) {
    static const void *dispatch_table[256] = {
        &&op_halt,
        &&op_load_constant,
        &&op_load_u8,

        &&op_invalid,
        &&op_invalid,
        &&op_invalid,

        [ HOLD_INSTRUCTION_SAVE_U8_TO_STACK ] = &&op_save_u8,

        [7 ... 255] = &&op_invalid,
    };

    #define HOLD_INSTRUCTION_LOGIC(logic)       \
    {                                           \
        instruction = *vm->pc++;                \
        op_code = (instruction >> 24) & 0xFF;   \
        logic;                                  \
        goto *dispatch_table[op_code];          \
    }                                           \

    #define HOLD_INSTRUCTION(name, logic)       \
        op_##name:                              \
        HOLD_INSTRUCTION_LOGIC(logic)           \

    uint32_t instruction = 0;
    uint8_t op_code = 0;

    // Start
    HOLD_INSTRUCTION_LOGIC({})

    HOLD_INSTRUCTION(halt, {
        return;
    })

    HOLD_INSTRUCTION(load_constant, {
        uint8_t register_index = (instruction & 0x00FF0000) >> 16;
        uint16_t constant_index = instruction & 0x0000FFFF;
        vm->reg_base[register_index] = vm->constants[constant_index];
    })

    HOLD_INSTRUCTION(load_u8, {
        uint8_t register_index = (instruction & 0x00FF0000) >> 16;
        uint16_t stack_offset = instruction & 0x0000FFFF;
        vm->reg_base[register_index].u64 = *(vm->sp_base + stack_offset);
    })

    HOLD_INSTRUCTION(save_u8, {
        uint8_t register_index = (instruction & 0x00FF0000) >> 16;
        uint16_t stack_offset = instruction & 0x0000FFFF;
        *(vm->sp_base + stack_offset) = vm->reg_base[register_index].u8;
    })

op_invalid:
    printf("Invalid op: '0x%02x' at index %ld.\n", op_code, vm->pc - vm->code - 1);
    return;

    #undef HOLD_INSTRUCTION
}

static inline void hold_push_u8(HoldVM *vm, uint8_t value) {
    *(vm->sp++) = value;
}

static inline void hold_push_u16(HoldVM *vm, uint16_t value) {
    vm->sp = (uint8_t*)HOLD_ALIGN_MEMORY(vm->sp, 2);
    *(uint16_t*)(vm->sp++) = value;
}

static inline void hold_push_u32(HoldVM *vm, uint32_t value) {
    vm->sp = (uint8_t*)HOLD_ALIGN_MEMORY(vm->sp, 4);
    *(uint32_t*)(vm->sp++) = value;
}

static inline void hold_push_u64(HoldVM *vm, uint64_t value) {
    vm->sp = (uint8_t*)HOLD_ALIGN_MEMORY(vm->sp, 8);
    *(uint64_t*)(vm->sp++) = value;
}

static inline void hold_push_f32(HoldVM *vm, float value) {
    vm->sp = (uint8_t*)HOLD_ALIGN_MEMORY(vm->sp, 4);
    *(float*)(vm->sp++) = value;
}

static inline void hold_push_f64(HoldVM *vm, double value) {
    vm->sp = (uint8_t*)HOLD_ALIGN_MEMORY(vm->sp, 8);
    *(double*)(vm->sp++) = value;
}

static inline void hold_push_obj(HoldVM *vm, HoldObject* obj) {
    *(vm->osp++) = obj;
}
