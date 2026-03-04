#include "vm.h"
#include "gc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN(address, to) ((((uintptr_t)address) + (to - 1)) & ~(to - 1));

void hold_init_vm(HoldVM *vm, uint32_t *code) {
    vm->stack = malloc(STACK_SIZE * sizeof(*vm->stack));
    vm->object_stack = malloc(STACK_SIZE * sizeof(*vm->object_stack));
    vm->code = code;
    vm->pc = vm->code;
    vm->sp = vm->stack;
    vm->osp = vm->object_stack;
    vm->frame_stack = malloc(STACK_SIZE * sizeof(*vm->frame_stack));
    vm->frame_ptr = vm->frame_stack;
    vm->frame_ptr->osp = vm->osp;
    vm->frame_ptr->sp = vm->sp;
    vm->frame_ptr->pc = vm->pc;

    hold_gc_init(&vm->gc);

    memset(vm->registers, 0, REGISTER_COUNT);
}

void hold_free_vm(HoldVM *vm) {
    hold_gc_free(&vm->gc);
    free(vm->stack);
    free(vm->object_stack);
    free(vm->frame_stack);
}

void hold_tick(HoldVM *vm) {
    uint32_t instruction = *(vm->pc++);
    uint8_t op_code = (instruction & 0xFF000000) >> 24;
    uint32_t destination_index;
    uint32_t source_index1;
    uint32_t source_index2;

    switch (op_code) {
        case HOLD_INSTRUCTION_HALT:
            vm->running = false;
            vm->pc--;
        break;
        case HOLD_INSTRUCTION_LOAD_CONSTANT:
             destination_index = (instruction & 0x00FF0000) >> 16;
             source_index1 = instruction & 0x0000FFFF;
             vm->registers[destination_index] = vm->constants[source_index1];
        break;
        case HOLD_INSTRUCTION_LOAD_U8_FROM_STACK:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = instruction & 0x0000FFFF;
            vm->registers[destination_index].u64 = *((vm->frame_ptr - 1)->sp + source_index1);
        break;
        case HOLD_INSTRUCTION_LOAD_U16_FROM_STACK:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = instruction & 0x0000FFFF;
            vm->registers[destination_index].u64 = *(uint16_t*)((vm->frame_ptr - 1)->sp + source_index1);
        break;
        case HOLD_INSTRUCTION_LOAD_U32_FROM_STACK:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = instruction & 0x0000FFFF;
            vm->registers[destination_index].u64 = *(uint32_t*)((vm->frame_ptr - 1)->sp + source_index1);
        break;
        case HOLD_INSTRUCTION_LOAD_U64_FROM_STACK:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = instruction & 0x0000FFFF;
            vm->registers[destination_index].u64 = *(uint64_t*)((vm->frame_ptr - 1)->sp + source_index1);
        break;
        case HOLD_INSTRUCTION_SAVE_U8_TO_STACK:
            destination_index = (instruction & 0x00FFFF00) >> 8;
            source_index1 = instruction & 0x000000FF;
            *(uint8_t*)((vm->frame_ptr - 1)->sp + destination_index) = (uint8_t)vm->registers[source_index1].u64;
        break;
        case HOLD_INSTRUCTION_SAVE_U16_TO_STACK:
            destination_index = (instruction & 0x00FFFF00) >> 8;
            source_index1 = instruction & 0x000000FF;
            *(uint16_t*)((vm->frame_ptr - 1)->sp + destination_index) = (uint16_t)vm->registers[source_index1].u64;
        break;
        case HOLD_INSTRUCTION_ADD_INTEGER:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = (instruction & 0x0000FF00) >> 8;
            source_index2 = instruction & 0x000000FF;
            vm->registers[destination_index].u64 = vm->registers[source_index1].u64 + vm->registers[source_index2].u64;
        break;
        case HOLD_INSTRUCTION_ADD_F64:
            destination_index = (instruction & 0x00FF0000) >> 16;
            source_index1 = (instruction & 0x0000FF00) >> 8;
            source_index2 = instruction & 0x000000FF;
            vm->registers[destination_index].f64 = vm->registers[source_index1].f64 + vm->registers[source_index2].f64;
        break;
        case HOLD_INSTRUCTION_EQ_INTEGER:
            source_index1 = (instruction & 0x00FF0000) >> 16;
            source_index2 = (instruction & 0x0000FF00) >> 8;
            if (vm->registers[source_index1].u64 != vm->registers[source_index2].u64) {
                vm->pc++;
            }
        break;
        case HOLD_INSTRUCTION_EQ_FLOAT:
            source_index1 = (instruction & 0x00FF0000) >> 16;
            source_index2 = (instruction & 0x0000FF00) >> 8;
            if (vm->registers[source_index1].f64 != vm->registers[source_index2].f64) {
                vm->pc++;
            }
        break;
        case HOLD_INSTRUCTION_JUMP: {
            uintptr_t offset = (uintptr_t)(instruction & 0x00FFFFFF) - 0x00800000;
            vm->pc += (uintptr_t)offset;
        } break;
        case HOLD_INSTRUCTION_CALL: {
            uintptr_t offset = (uintptr_t)(instruction & 0x00FFFFFF) - 0x00800000;
            vm->frame_ptr->sp = vm->sp;
            vm->frame_ptr->osp = vm->osp;
            vm->frame_ptr->pc = vm->pc;
            vm->frame_ptr++;
            vm->pc += (uintptr_t)offset;
        } break;
        case HOLD_INSTRUCTION_RETURN: {
            uintptr_t offset = (uintptr_t)(instruction & 0x00FFFFFF) - 0x00800000;
            vm->frame_ptr--;
            vm->sp = vm->frame_ptr->sp;
            vm->osp = vm->frame_ptr->osp;
            vm->pc = vm->frame_ptr->pc;
        } break;
        case HOLD_INSTRUCTION_ALLOC_OBJECT: {
            uint32_t bytes = instruction & 0x00FFFFFF;
            HoldObject* obj = malloc(sizeof(HoldObject) + bytes);
            *(vm->osp++) = obj;
            hold_gc_track(&vm->gc, vm, obj);
        } break;
    }
}

void hold_push_u8(HoldVM *vm, uint8_t value) {
    *(vm->sp++) = value;
}

void hold_push_u16(HoldVM *vm, uint16_t value) {
    vm->sp = (uint8_t*)ALIGN(vm->sp, 2);
    *(uint16_t*)(vm->sp++) = value;
}

void hold_push_u32(HoldVM *vm, uint32_t value) {
    vm->sp = (uint8_t*)ALIGN(vm->sp, 4);
    *(uint32_t*)(vm->sp++) = value;
}

void hold_push_u64(HoldVM *vm, uint64_t value) {
    vm->sp = (uint8_t*)ALIGN(vm->sp, 8);
    *(uint64_t*)(vm->sp++) = value;
}

void hold_push_f32(HoldVM *vm, float value) {
    vm->sp = (uint8_t*)ALIGN(vm->sp, 4);
    *(float*)(vm->sp++) = value;
}

void hold_push_f64(HoldVM *vm, double value) {
    vm->sp = (uint8_t*)ALIGN(vm->sp, 8);
    *(double*)(vm->sp++) = value;
}

void hold_push_obj(HoldVM *vm, HoldObject* obj) {
    *(vm->osp++) = obj;
}
