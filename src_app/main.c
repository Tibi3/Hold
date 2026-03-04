#include "vm.h"
#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    HoldVM vm;

    uint32_t instructions[] = {
        // LOAD_CONSTANT TO REG 0 FROM INDEX 0
        0x01000000,
        // Call
        0x10800000 + 4,
        // Halt
        0x00000000,
        0x01000010,
        0x01000010,
        0x01000010,
        // LOAD_CONSTANT TO REG 0 FROM INDEX 1
        0x01000001,
        // RETURN
        0x11000000,
    };

    hold_init_vm(&vm, instructions);

    vm.constants[0].f64 = 0.25;
    vm.constants[1].f64 = 0.26;
    vm.constants[2].f64 = 0.27;

    vm.running = true;
    for (int i = 0; i < 10; i++) {
        if (!vm.running) break;

        hold_tick(&vm);
        printf("reg 0 = %lf pc = %p\n", vm.registers[0].f64, vm.pc);
    }

    hold_free_vm(&vm);

    return 0;
}
