#ifndef __IFMO_DISTRIBUTED_CLASS_PA1__IO__H
#define __IFMO_DISTRIBUTED_CLASS_PA1__IO__H

#include "ipc.h"
#include "child.h"

enum {
    MAX_PROCESSES = 10,
};

Child self;
size_t num_processes;
size_t num_children;
size_t num_children;
size_t reader[MAX_PROCESSES][MAX_PROCESSES];
size_t writer[MAX_PROCESSES][MAX_PROCESSES];

int send_message_to_all(MessageType);
int receive_messages_from_all(MessageType);

#endif  // __IFMO_DISTRIBUTED_CLASS_PA1_IO__H
