#include <iostream>
#include <signal.h>
#include "../BaseServer.h"
#include "../eventLoop.h"
#include "../SingletonImp.h"

using namespace std;

int main()
{
    struct sigaction action;  
    action.sa_handler = SIG_IGN;  
    sigemptyset(&action.sa_mask);
    // sigaction(SIGINT, &action, NULL);
    sigaction(SIGALRM, &action, NULL);
    auto loop = SingletonImp<EventLoop>::getInstance();
    Socket soc("0.0.0.0", 8081, loop);
    soc.server();
    loop->addReadEvent(&soc);
    soc.setEvent(AOSLISTEN|AOSREAD);
    while (true)
    {
        loop->run();
    }
    sleep(10);
}
