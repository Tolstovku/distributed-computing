#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ipc.h"
#include "log.h"

int send_message_to_all(MessageType type)
{
        Message msg = {
            .s_header =
                {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = type,
                },
        };
        if(type == STARTED)
        {
            sprintf(msg.s_payload, log_started_fmt, local_pid, getpid(), getppid());
            log_started();
        } else
        {
            sprintf(msg.s_payload, log_done_fmt, local_pid);
            log_done();
        }
        msg.s_header.s_payload_len = strlen(msg.s_payload);
        send_multicast(NULL, &msg);
        return 0;
}

int receive_messages_from_all(MessageType type)
{
    for (size_t i = 1; i <= num_children; i++) {
        Message msg;
        if (i == local_pid) {
            continue;
        }
        receive(NULL, i, &msg);
    }
    if (type == STARTED){
        log_received_all_started();
    }
    else {
        log_received_all_done();
    }

    return 0;
}
