#pragma once

#include <stdint.h>
#include <stdalign.h>
#include "common.h"

#define STACK_SIZE 1024
#define REGISTER_COUNT 16
#define CONSTANT_COUNT 1024

enum HoldInstruction : uint8_t {
    // Load
    HOLD_INSTRUCTION_LOAD_CONSTANT,
    HOLD_INSTRUCTION_LOAD_U8_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U16_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U32_FROM_STACK,
    HOLD_INSTRUCTION_LOAD_U64_FROM_STACK,

    // Save
    HOLD_INSTRUCTION_SAVE_U8_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U16_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U32_TO_STACK,
    HOLD_INSTRUCTION_SAVE_U64_TO_STACK,

    // Arithmetic
    HOLD_INSTRUCTION_ADD_INTEGER,
    HOLD_INSTRUCTION_ADD_F32,
    HOLD_INSTRUCTION_ADD_F64,
};

typedef union {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
} HoldRegister;

typedef struct {
    // The GC don't know what fields this object has so we push all ptr types at the back.
    // After object_ptrs_start_at we know there are only ptrs the GC can follow.
    uint16_t object_ptrs_start_at;
    bool marked;
    alignas(8)
    uint8_t fields[];
} HoldObject;

typedef struct {
    uint8_t *sp;
    HoldObject **osp;
    uint32_t *pc;
} HoldFrame;

typedef struct {
    HoldRegister registers[REGISTER_COUNT];
    uint32_t *pc;
    uint8_t *sp;
    HoldFrame *frame_ptr;
    HoldObject **osp;
    uint32_t *code;
    uint8_t *stack;
    HoldObject **object_stack;
    HoldFrame *frame_stack;
    HoldRegister constants[CONSTANT_COUNT];
} HoldVM;

void hold_init_vm(HoldVM *vm, uint32_t *code);
void hold_free_vm(HoldVM *vm);

void hold_tick(HoldVM *vm);

void hold_push_u8(HoldVM *vm, uint8_t value);
void hold_push_u16(HoldVM *vm, uint16_t value);
void hold_push_u32(HoldVM *vm, uint32_t value);
void hold_push_u64(HoldVM *vm, uint64_t value);
void hold_push_f32(HoldVM *vm, float value);
void hold_push_f64(HoldVM *vm, double value);
void hold_push_obj(HoldVM *vm, HoldObject* obj);
