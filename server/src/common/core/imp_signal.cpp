#include <sys/syscall.h>
#include "signal.h"
#include "log.h"

CSignal::CSignal()
{
}

CSignal::~CSignal()
{
}

void CSignal::Init()
{
    // signal ingore
    uint ignores[] = { SIGIO, SIGPIPE, SIGPOLL, SIGPROF, SIGURG, SIGVTALRM, SIGCHLD, SIGCONT, SIGHUP };
    sigset_t mask;
    sigemptyset(&mask);
    uint count = sizeof(ignores) / sizeof(uint);
    FOR(count, i)
    {
        signal(ignores[i], SIG_IGN);
        sigaddset(&mask, ignores[i]);
    }
    if(sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
    {
        LOG_ERROR("sigprocmask error!");
    }

    // signal generate core file
    /*
    signal(SIGABRT, CSignal::OnCore);
    signal(SIGILL,  CSignal::OnCore);
    signal(SIGSEGV, CSignal::OnCore);
    signal(SIGFPE,  CSignal::OnCore);
    signal(SIGSYS,  CSignal::OnCore);
    */

    // signal user define
    signal(SIGUSR1, CSignal::OnUsers);
    signal(SIGUSR2, CSignal::OnUsers);
}

void CSignal::OnCore(int signo)
{
    printf("++++++++++++OnCore:%u++++++++++++++++++++\n", signo);
    LOG_ERROR("++++++++++++OnCore:%u++++++++++++++++++++", signo);

    // gen core file
    char buff[64];
    snprintf(buff, sizeof(buff) - 1, "pstack %u > %s.stack", (uint)syscall(SYS_gettid), "core");
    system(buff);

    #ifdef DEBUG
        signal(signo, SIG_DFL);
    #endif

    // seg failt always fatal error in a program, may be impossible to fix.
    if(signo == SIGSEGV)
    {
        LOG_ERROR("Fatal failure[SIGSEGV], can't be fixed, must to exit.");
        exit(-1);
    }
}

void CSignal::OnUsers(int signo)
{
    LOG_ERROR("==========OnUsers:%u, BYE ==========", signo);
    EVENT(SEventSignalUser, signo);
    exit(0);
}
