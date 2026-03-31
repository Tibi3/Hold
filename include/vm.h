#pragma once

#include <stdint.h>
#include <stdalign.h>
#include "common.h"
#include "gc.h"

#define HOLD_STACK_SIZE 4096
#define HOLD_REGISTER_COUNT 4096

enum HoldInstruction : uint8_t {
    HOLD_INSTRUCTION_HALT,
    // Load
    // Currently the constant table is HoldRegister, whitch means we can load any type with this one instruction.
    // TODO: determine if this wastes too much memory.
    HOLD_INSTRUCTION_LOAD_CONSTANT,
    HOLD_INSTRUCTION_LOAD_U8_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U16_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U32_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U64_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_I8_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_I16_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_I32_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_I64_FROM_STACK,

    HOLD_INSTRUCTION_LOAD_F32_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_F64_FROM_STACK,

    // Save
    HOLD_INSTRUCTION_SAVE_U8_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U16_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U32_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U64_TO_STACK,
    HOLD_INSTRUCTION_SAVE_I8_TO_STACK,
    HOLD_INSTRUCTION_SAVE_I16_TO_STACK,
    HOLD_INSTRUCTION_SAVE_I32_TO_STACK,
    HOLD_INSTRUCTION_SAVE_I64_TO_STACK,

    HOLD_INSTRUCTION_SAVE_F32_TO_STACK,
    HOLD_INSTRUCTION_SAVE_F64_TO_STACK,

    // Arithmetic

    // In some cases the underlying logic for signed and unsigned operations is the same.
    // In those cases we don't have to implement the signed counterpart.
    // Also 8/16 bit types are casted to 32bit in the register.
    HOLD_INSTRUCTION_ADD_U32,
    HOLD_INSTRUCTION_ADD_U64,
    HOLD_INSTRUCTION_SUB_U32,
    HOLD_INSTRUCTION_SUB_U64,

    HOLD_INSTRUCTION_MUL_U32,
    HOLD_INSTRUCTION_MUL_U64,
    HOLD_INSTRUCTION_MUL_I32,
    HOLD_INSTRUCTION_MUL_I64,

    HOLD_INSTRUCTION_DIV_U32,
    HOLD_INSTRUCTION_DIV_U64,
    HOLD_INSTRUCTION_DIV_I32,
    HOLD_INSTRUCTION_DIV_I64,

    HOLD_INSTRUCTION_MOD_U32,
    HOLD_INSTRUCTION_MOD_U64,
    HOLD_INSTRUCTION_MOD_I32,
    HOLD_INSTRUCTION_MOD_I64,

    HOLD_INSTRUCTION_LSH_U32,
    HOLD_INSTRUCTION_LSH_U64,
    HOLD_INSTRUCTION_RSH_U32,
    HOLD_INSTRUCTION_RSH_U64,
    // Apperentaly right shift preserves the signed bit.
    HOLD_INSTRUCTION_RSH_I32,
    HOLD_INSTRUCTION_RSH_I64,

    HOLD_INSTRUCTION_OR_U32,
    HOLD_INSTRUCTION_OR_U64,
    HOLD_INSTRUCTION_AND_U32,
    HOLD_INSTRUCTION_AND_U64,
    HOLD_INSTRUCTION_XOR_U32,
    HOLD_INSTRUCTION_XOR_U64,
    HOLD_INSTRUCTION_NOT_U32,
    HOLD_INSTRUCTION_NOT_U64,

    HOLD_INSTRUCTION_ADD_F32,
    HOLD_INSTRUCTION_ADD_F64,
    HOLD_INSTRUCTION_SUB_F32,
    HOLD_INSTRUCTION_SUB_F64,
    HOLD_INSTRUCTION_MUL_F32,
    HOLD_INSTRUCTION_MUL_F64,
    HOLD_INSTRUCTION_DIV_F32,
    HOLD_INSTRUCTION_DIV_F64,

    HOLD_INSTRUCTION_JUMP,

    HOLD_INSTRUCTION_CALL,
    HOLD_INSTRUCTION_RETURN,

    HOLD_INSTRUCTION_ALLOC_OBJECT,
    HOLD_INSTRUCTION_OBJECT_SET_FIELD,
    HOLD_INSTRUCTION_OBJECT_GET_FIELD,
};

typedef union HoldRegister {
    int32_t i32;
    int64_t i64;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
} HoldRegister;

typedef struct HoldObject {
    // The GC don't know what fields this object has so we push all ptr types at the front.
    // After object_ptrs_count we know there are no ptrs.
    uint16_t object_ptrs_count;
    bool marked;
    alignas(8)
    uint8_t fields[];
} HoldObject;

typedef struct HoldVM {
    HoldRegister registers[HOLD_REGISTER_COUNT];
    // Stores primitive types.
    uint8_t stack[HOLD_STACK_SIZE];
    // Stores Object pointers. The GC has to scan this stack only.
    HoldObject* object_stack[HOLD_STACK_SIZE];
    uint8_t *sp;
    uint8_t *sp_base;
    HoldObject **osp;
    HoldObject **osp_base;
    HoldRegister *reg_base;
    const uint32_t *pc;
    const uint32_t *code;
    // TODO: move constants into code, add code header, add type info table to code
    const HoldRegister *constants;
    GC gc;
} HoldVM;

void hold_init_vm(HoldVM *vm, const uint32_t *code, const HoldRegister* constants);
void hold_free_vm(HoldVM *vm);

void hold_run(HoldVM *vm);
void hold_tick(HoldVM *vm);
