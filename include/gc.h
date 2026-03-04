#pragma once

#include <stdint.h>

typedef struct HoldObject HoldObject;
typedef struct HoldVM HoldVM;

typedef struct {
    struct HoldObject **tracked_objects;
    uint64_t tracked_object_count;
    uint64_t tracked_object_capacity;
} GC;

void hold_gc_init(GC *gc);
void hold_gc_free(GC *gc);
void hold_gc_track(GC *gc, HoldVM *vm, HoldObject* object);
void hold_gc_mark_and_sweep(GC *gc, HoldVM *vm);
