#include "ipc.h"
#include <unistd.h>
#include "io.h"
#include "stdio.h"

static size_t read_bytes(size_t fd, void *buf, size_t nbytes);

typedef enum {
    INVALID_PEER = 1,
    INVALID_MAGIC,
} msg_error;

int send(void *self, local_id dst, const Message *msg) {
    if (dst >= num_processes) {
        return INVALID_PEER;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return INVALID_MAGIC;
    }
    write(writer[local_pid][dst], &msg->s_header, sizeof(MessageHeader));
    write(writer[local_pid][dst], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    for (local_id dst = 0; dst < num_processes; dst++) {
        if (dst != local_pid) {
            int result = send(self, dst, msg);
            if (result > 0) {
                return result;
            }
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    if (from >= num_processes) {
        return INVALID_PEER;
    }

    read_bytes(reader[from][local_pid], &msg->s_header, sizeof(MessageHeader));
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return INVALID_MAGIC;
    }

    read_bytes(reader[from][local_pid], &msg->s_payload,
               msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    for (local_id dst = 0; dst < num_processes; dst++) {
        if (dst != local_pid) {
            int result = receive(self, dst, msg);
            if (result > 0) {
                return result;
            }
        }
    }
    return 0;
}

/**
 * Attempts to read the exact number of bytes from file into the buffer.
 *
 * Unlike read(3P), this function will either read the exact given number of
 * bytes or exit with an error.
 */
static size_t read_bytes(size_t fd, void *buf, size_t num_bytes) {
    int bytes_read = read(fd, ((char *) buf), num_bytes);
    if (bytes_read < num_bytes)
    {
        return -1;
    }
    return bytes_read;
}
