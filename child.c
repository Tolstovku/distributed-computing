#include "ipc.h"
#include "pa1.h"
#include "io.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "io.h"

int run_child_routine()
{
    send_message_to_all(STARTED);
    receive_messages_from_all(STARTED);
    send_message_to_all(DONE);
    receive_messages_from_all(DONE);
    return 0;
}
