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
    #define HOLD_INSTRUCTION_LOGIC(logic)       \
    {                                           \
        logic;                                  \
        instruction = *vm->pc++;                \
        op_code = (instruction >> 24) & 0xFF;   \
        goto *dispatch_table[op_code];          \
    }                                           \

    #define HOLD_INSTRUCTION(name, logic)       \
        op_##name:                              \
        HOLD_INSTRUCTION_LOGIC(logic)           \

    #define TARGET_REG      ((instruction & 0x00FF0000) >> 16)
    #define TARGET_INDEX    ((instruction & 0x00FFFF00) >> 8)
    #define SOURCE_REG      (instruction & 0x000000FF)
    #define SOURCE_REG2     ((instruction & 0x0000FF00) >> 8)
    #define SOURCE_INDEX    (instruction & 0x0000FFFF)

    static const void *dispatch_table[256] = {
        &&op_halt,
        &&op_load_constant,
        &&op_load_u8,
        &&op_load_u16,
        &&op_load_u32,
        &&op_load_u64,
        &&op_load_f32,
        &&op_load_f64,
        &&op_save_u8,
        &&op_save_u16,
        &&op_save_u32,
        &&op_save_u64,
        &&op_save_f32,
        &&op_save_f64,

        [14 ... 255] = &&op_invalid,
    };

    // [ op_code(8bit) | target_reg(8bit) |      - (8bit)     | source_reg(8bit) ]
    // [ op_code(8bit) | target_reg(8bit) | source_reg2(8bit) | source_reg(8bit) ]
    // [ op_code(8bit) | target_reg(8bit) |          source_index(16bit)         ]
    // [ op_code(8bit) |          target_index(16bit)         | source_reg(8bit) ]
    // [ op_code(8bit) |                        ? (24bit)                        ]
    uint32_t instruction = 0;
    uint8_t op_code = 0;
    // TODO: AFTER making stuff work investigate this:
    // apparently puting vm->* into local variables can help with performance, also can introduce desync.
    const HoldRegister *constants = vm->constants;

    // Start
    HOLD_INSTRUCTION_LOGIC({})

    HOLD_INSTRUCTION(halt, {
        return;
    })

    HOLD_INSTRUCTION(load_constant, {
        vm->reg_base[TARGET_REG] = constants[SOURCE_INDEX];
    })

    HOLD_INSTRUCTION(load_u8, {
        vm->reg_base[TARGET_REG].u8 = *(vm->sp_base + SOURCE_INDEX);
    })

    HOLD_INSTRUCTION(load_u16, {
        vm->reg_base[TARGET_REG].u16 = *((uint16_t*)(vm->sp_base + SOURCE_INDEX));
    })

    HOLD_INSTRUCTION(load_u32, {
        vm->reg_base[TARGET_REG].u32 = *((uint32_t*)(vm->sp_base + SOURCE_INDEX));
    })

    HOLD_INSTRUCTION(load_u64, {
        vm->reg_base[TARGET_REG].u64 = *((uint64_t*)(vm->sp_base + SOURCE_INDEX));
    })

    HOLD_INSTRUCTION(load_f32, {
        vm->reg_base[TARGET_REG].f32 = *((float*)(vm->sp_base + SOURCE_INDEX));
    })

    HOLD_INSTRUCTION(load_f64, {
        vm->reg_base[TARGET_REG].f64 = *((double*)(vm->sp_base + SOURCE_INDEX));
    })

    HOLD_INSTRUCTION(save_u8, {
        *(vm->sp_base + TARGET_INDEX) = vm->reg_base[SOURCE_REG].u8;
    })

    HOLD_INSTRUCTION(save_u16, {
        *((uint16_t*)(vm->sp_base + TARGET_INDEX)) = vm->reg_base[SOURCE_REG].u16;
    })

    HOLD_INSTRUCTION(save_u32, {
        *((uint32_t*)(vm->sp_base + TARGET_INDEX)) = vm->reg_base[SOURCE_REG].u32;
    })

    HOLD_INSTRUCTION(save_u64, {
        *((uint64_t*)(vm->sp_base + TARGET_INDEX)) = vm->reg_base[SOURCE_REG].u64;
    })

    HOLD_INSTRUCTION(save_f32, {
        *((float*)(vm->sp_base + TARGET_INDEX)) = vm->reg_base[SOURCE_REG].f32;
    })

    HOLD_INSTRUCTION(save_f64, {
        *((double*)(vm->sp_base + TARGET_INDEX)) = vm->reg_base[SOURCE_REG].f64;
    })

op_invalid:
    printf("Invalid op: '0x%02x' at index %ld.\n", op_code, vm->pc - vm->code - 1);
    return;

    #undef HOLD_INSTRUCTION_LOGIC
    #undef HOLD_INSTRUCTION
    #undef TARGET_REG
    #undef TARGET_INDEX
    #undef SOURCE_REG
    #undef SOURCE_REG2
    #undef SOURCE_INDEX
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
