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

    //child - запускается, отсылает старты, переходит в режим 
    //ожидания либо трансфера либо стопа либо DONE,
    //если стоп - принимать только трансферы где он dst , 
    //но не src, ждет все DONE, отсылает историю родителю, умирает

    send_message_to_all(DONE);
    receive_messages_from_all(DONE);
    return 0;
}
