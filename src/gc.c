#include "gc.h"
#include "vm.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>

#define TRIGGER_CLEAN_UP_AT 1024

void hold_gc_init(GC *gc) {
    gc->tracked_object_count = 0;
    gc->tracked_object_capacity = 512;
    gc->tracked_objects = malloc(sizeof(*gc->tracked_objects) * gc->tracked_object_capacity);
}

void hold_gc_free(GC *gc) {
    // TODO: warn if tracked_object_count > 0
    free(gc->tracked_objects);
}

void hold_gc_track(GC *gc, HoldVM *vm, HoldObject *object) {
    if (gc->tracked_object_count > TRIGGER_CLEAN_UP_AT) {
        hold_gc_mark_and_sweep(gc, vm);
    }

    if (gc->tracked_object_count >= gc->tracked_object_capacity) {
        gc->tracked_object_capacity *= 2;
        gc->tracked_objects = realloc(gc->tracked_objects, sizeof(*gc->tracked_objects) * gc->tracked_object_capacity);
    }

    gc->tracked_objects[gc->tracked_object_count++] = object;
}

static void mark_object(GC *gc, HoldObject* op);

void hold_gc_mark_and_sweep(GC *gc, HoldVM *vm) {
    for (HoldObject** op = vm->frame_stack[0].osp; op < vm->osp; op++) {
        mark_object(gc, *op);
    }

    uint64_t i = 0;
    while(i < gc->tracked_object_count) {
        if (!(*gc->tracked_objects[i]).marked) {
            free(gc->tracked_objects[i]);

            gc->tracked_object_count--;
            if (gc->tracked_object_count > 0) {
                gc->tracked_objects[i] = gc->tracked_objects[gc->tracked_object_count];
                continue;
            }
        }

        (*gc->tracked_objects[i]).marked = false;
        i++;
    }
}

static void mark_object(GC *gc, HoldObject* op) {
    if (op->marked) return;

    op->marked = true;
    for (uint16_t i = 0; i < op->object_ptrs_count; i++) {
        uintptr_t *ptr_array = (uintptr_t*)op->fields;
        mark_object(gc, (HoldObject*)ptr_array[i]);
    }
}
