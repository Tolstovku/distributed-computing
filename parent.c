#include "ipc.h"
#include "pa1.h"
#include "io.h"
#include <sys/types.h>
#include <sys/wait.h>

int wait_for_children_to_stop(pid_t*);

int run_parent_routine(pid_t* pids){
    receive_messages_from_all(STARTED);
    receive_messages_from_all(DONE);
    wait_for_children_to_stop(pids);
    return 0;
}

int wait_for_children_to_stop(pid_t* pids){
    for (size_t i = 1; i <= num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
