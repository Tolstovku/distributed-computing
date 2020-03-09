#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "io.h"
#include "ipc.h"
#include "log.h"

void close_pipes_that_dont_belong_to_us();

int main(int argc, char const *argv[]) {
    size_t num_children;

    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        num_children = strtol(argv[2], NULL, 10);
    } else {
        fprintf(stderr, "Error: Key '-p NUMBER_OF_CHILDREN' is mandatory\n");
        return 1;
    }

    if (num_children >= MAX_PROCESSES) {
        fprintf(stderr, "Error: Too many children requested.\n");
        return 1;
    }

    num_processes = num_children + 1;

    log_init();

    //  ------------ Create file descriptors. ------------
    for (size_t source = 0; source < num_processes; source++) {
        for (size_t destination = 0; destination < num_processes;
             destination++) {
            if (source != destination) {
                int fildes[2];
                pipe(fildes);
                reader[source][destination] = fildes[0];
                writer[source][destination] = fildes[1];
                log_pipe_opened(source, destination);
            }
        }
    }


    pid_t process_pids[num_children];
    process_pids[PARENT_ID] = getpid();

    // ------------ Create children processes ------------
    for (size_t id = 1; id <= num_children; id++) {
        int child_pid = fork();
        if (child_pid > 0) {
            // We are inside the parent process.
            local_pid = PARENT_ID;
            process_pids[id] = child_pid;
        } else if (child_pid == 0) {
            // We are inside the child process.
            local_pid = id;
            break;
        } else {
            fprintf(stderr, "Unexpected fork() error");
            return -1;
        }
    }

    close_pipes_that_dont_belong_to_us();

    //  ------------ Send STARTED message ------------
    if (local_pid != PARENT_ID) {
        Message msg = {
            .s_header =
                {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = STARTED,
                },
        };
        sprintf(msg.s_payload, log_started_fmt, local_pid, getpid(), getppid());
        msg.s_header.s_payload_len = strlen(msg.s_payload);
        send_multicast(NULL, &msg);
        log_started();
    }

    // ------------ Recieve STARTED message ------------
    for (size_t i = 1; i <= num_children; i++) {
        Message msg;
        if (i == local_pid) {
            continue;
        }
        receive(NULL, i, &msg);
        printf("I %d Received from %2d\n ", local_pid, i);
    }
    log_received_all_started();

    // ------------ Send DONE message ------------
    if (local_pid != PARENT_ID) {
        Message msg = {
            .s_header =
                {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = DONE,
                },
        };
        sprintf(msg.s_payload, log_done_fmt, local_pid);
        msg.s_header.s_payload_len = strlen(msg.s_payload);
        send_multicast(NULL, &msg);
        log_done();
    }

    // ------------ Recieve DONE message ------------
    for (size_t i = 1; i <= num_children; i++) {
        Message msg;
        if (i == local_pid) {
            continue;
        }
        receive(NULL, i, &msg);
    }
    log_received_all_done();

    if (local_pid == PARENT_ID) {
        // Wait for the children to stop
        for (size_t i = 1; i <= num_processes; i++) {
            waitpid(process_pids[i], NULL, 0);
        }
    }

    log_close();
    return 0;
}  // main

void close_pipes_that_dont_belong_to_us() {
    for (size_t source = 0; source < num_processes; source++) {
        for (size_t destination = 0; destination < num_processes;
             destination++) {
            if (source != local_pid && destination != local_pid &&
                source != destination) {
                close(writer[source][destination]);
                close(reader[source][destination]);
            }
            if (source == local_pid && destination != local_pid) {
                close(reader[source][destination]);
            }
            if (destination == local_pid && source != local_pid) {
                close(writer[source][destination]);
            }
        }
    }
}
