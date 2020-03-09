#ifndef __IFMO_DISTRIBUTED_CLASS_PA1__IO__H
#define __IFMO_DISTRIBUTED_CLASS_PA1__IO__H

#include "ipc.h"

enum {
    MAX_PROCESSES = 10,
};

local_id local_pid;
size_t num_processes;
size_t reader[MAX_PROCESSES][MAX_PROCESSES];
size_t writer[MAX_PROCESSES][MAX_PROCESSES];

#endif  // __IFMO_DISTRIBUTED_CLASS_PA1_IO__H
