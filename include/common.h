#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define HOLD_ALIGN_MEMORY(address, to) ((((uintptr_t)address) + (to - 1)) & ~(to - 1));
