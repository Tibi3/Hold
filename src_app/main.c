#include "vm.h"
#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    HoldVM vm;

    uint32_t instructions[] = {
        // LOAD_CONSTANT TO REG 0 FROM INDEX 0
        0x00000000,
        // LOAD_CONSTANT TO REG 1 FROM INDEX 1
        0x00010001,
        // ADD REG 0 and REG 1 STORE IT REG 2
        0x0B020001
    };

    hold_init_vm(&vm, instructions);

    vm.constants[0].f64 = 0.25;
    vm.constants[1].f64 = 0.26;

    printf("%ld\n", sizeof(HoldObject));
    HoldObject *x = malloc(sizeof(HoldObject) + 16);

    ((uint64_t*)(x->fields))[0] = 5;
    ((uint64_t*)(x->fields))[1] = 6;

    printf("%ld %ld\n", ((uint64_t*)(x->fields))[0], ((uint64_t*)(x->fields))[1]);

    hold_tick(&vm);
    hold_tick(&vm);
    hold_tick(&vm);

    for (int i = 0; i < 3; i++) {
        printf("reg %d = %lf\n", i, vm.registers[i].f64);
    }

    hold_free_vm(&vm);

    return 0;
}
