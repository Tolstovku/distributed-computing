#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "child.h"
#include "parent.h"

#include "io.h"
#include "ipc.h"
#include "log.h"

void close_pipes_that_dont_belong_to_us();

int main(int argc, char const *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        num_children = strtol(argv[2], NULL, 10);
    } else {
        fprintf(stderr, "Error: Key '-p NUMBER_OF_CHILDREN' is mandatory\n");
        return 1;
    }

    if (num_children >= MAX_PROCESSES) {
        fprintf(stderr, "Error: There can't be more than 9 children.\n");
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

    if (local_pid == PARENT_ID) {
        run_parent_routine(process_pids);
    } else {
        run_child_routine();
    }

    log_close();
    return 0;
}

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
