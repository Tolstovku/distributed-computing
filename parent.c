#include "ipc.h"
#include "pa1.h"
#include "io.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "banking.h"
#include "pa2345.h"

int wait_for_children_to_stop(pid_t*);

int run_parent_routine(pid_t* pids){

    receive_messages_from_all(STARTED);

    bank_robbery(); // EVE define transfer

    send_message_to_all(STOP);

    receive_messages_from_all(DONE);

    AllHistory *all_history;
    recieve_balance_histories(all_history); // EVE DODO

    wait_for_children_to_stop(pids);

    print_history(all_history);
    return 0;
}

int wait_for_children_to_stop(pid_t* pids){
    for (size_t i = 1; i <= num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
