#ifndef _COMMON_CORE_SIGNAL_H_
#define _COMMON_CORE_SIGNAL_H_

#include <signal.h>
#include "event.h"

// SEventSignalUser
struct SEventSignalUser : public SEventBase
{
    uint type;
    SEventSignalUser(uint t) : type(t) { }
};

// CSignal
class CSignal
{
public:
    void Init();

private:
    friend CSignal& CSingleton<CSignal>::Instance();
    CSignal();
    ~CSignal();

private:
    static void OnCore(int signo);
    static void OnUsers(int signo);
};
#define theSignal CSingleton<CSignal>::Instance()

#endif

/*
#   SIGNAL NAME     DEFAULT ACTION      COMMENT                                                         POSIX
1   SIGHUP          Terminate           Hang up controlling terminal or process                         Yes
2   SIGINT          Terminate           Interrupt from keyboard                                         Yes
3   SIGQUIT         Dump                Quit from keyboard                                              Yes
4   SIGILL          Dump                Illegal instruction                                             Yes
5   SIGTRAP         Dump                Breakpoint for debugging                                        No
6   SIGABRT         Dump                Abnormal termination                                            Yes
6   SIGIOT          Dump                Equivalent to SIGABRT                                           No
7   SIGBUS          Dump                Bus error                                                       No
8   SIGFPE          Dump                Floating-point exception                                        Yes
9   SIGKILL         Terminate           Forced-process termination                                      Yes
10  SIGUSR1         Terminate           Available to processes                                          Yes
11  SIGSEGV         Dump                Invalid memory reference                                        Yes
12  SIGUSR2         Terminate           Available to processes                                          Yes
13  SIGPIPE         Terminate           Write to pipe with no readers                                   Yes
14  SIGALRM         Terminate           Real-timerclock                                                 Yes
15  SIGTERM         Terminate           Process termination                                             Yes
16  SIGSTKFL        T                   Terminate   Coprocessor stack error                             No
17  SIGCHLD         Ignore              Child process stopped or terminated, or got signal if traced    Yes
18  SIGCONT         Continue            Resume execution, if stopped                                    Yes
19  SIGSTOP         Stop                Stop process execution                                          Yes
20  SIGTSTP         Stop                Stop process issued from tty                                    Yes
21  SIGTTIN         Stop                Background process requires input                               Yes
22  SIGTTOU         Stop                Background process requires output                              Yes
23  SIGURG          Ignore              Urgent condition on socket                                      No
24  SIGXCPU         Dump                CPU time limit exceeded                                         No
25  SIGXFSZ         Dump                File size limit exceeded                                        No
26  SIGVTALRM       Terminate           Virtual timer clock                                             No
27  SIGPROF         Terminate           Profile timer clock                                             No
28  SIGWINCH        Ignore              Window resizing                                                 No
29  SIGIO           Terminate           I/O now possible                                                No
29  SIGPOLL         Terminate           Equivalent to SIGIO                                             No
30  SIGPWR          Terminate           Power supply failure                                            No
31  SIGSYS          Dump                Bad system call                                                 No
31  SIGUNUSED       Dump                Equivalent to SIGSYS                                            No


Terminate: The process is terminated (killed)
Dump:      The process is terminated (killed) and a core file containing its execution contextis created, if possible; this file may be used for debug purposes.
Ignore:    The signal is ignored.
Stop:      The process is stopped, i.e., put in the TASK_STOPPED state.
Continue:  If the process was stopped (TASK_STOPPED), it is put into the TASK_RUNNING state.
*/
