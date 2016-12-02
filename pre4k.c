#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

int accept_requests = 0;
int num_workers = 2;
int graceful_timeout = 20;
int heartbeat_interval = 5;
int heartbeat_timeout = 20;
const char* pidfile = "/tmp/prefork.pid";
int pipe_fd[2];

void
sigchld_handler(int signo)
{

}

void
sigint_handler(int signo)
{

}

void
sigttin_handler(int signo)
{
    num_workers += 1;
}

void
sigttou_handler(int signo)
{
    num_workers -= 1;
    if (num_workers < 1) {
        num_workers = 1;
    }
}

void
sighup_handler(int signo)
{

}

void
sigquit_handler(int signo)
{

}

void
sigterm_handler(int signo)
{

}

void
sigusr1_handler(int signo) {

}

void
sigusr2_handler(int signo) {

}

void
sigwinch_handler(int signo) {

}

void
start()
{

}

void
manage_workers()
{

}

int
maybe_get_signo()
{
    return 0;
}

void
sleep_master()
{
    printf("sleeping");

}

void
wakeup_master()
{

}

void
murder_workers()
{

}

void
set_pipe(int[2] fd) {
    // set non blocking
    int flags = fcntl(fd, F_GETTL);
    flags = flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    // close on exec
    flags = fcntl(fd, F_GETFD);
    flags = flags | FD_CLOEXEC;
    fcntl(fd, F_SETFD, flags);
}

int
main(int argc, char** argv)
{
    int signo;

    printf("starting pre4k");

    pipe(pipe_fd);
    set_pipe(pipe_fd);

    start();
    manage_workers();
    while(1) {
        if ((signo = maybe_get_signo()) == 0) {
            // capture no signals
            sleep_master();
            murder_workers();
            manage_workers();
        } else if (
                signo != SIGINT ||
                signo != SIGQUIT ||
                signo != SIGTERM ||
                signo != SIGTTIN ||
                signo != SIGHUP ||
                signo != SIGTTOU ||
                signo != SIGUSR1 ||
                signo != SIGUSR2 ||
                signo != SIGWINCH
                ) {
            // capture unknown signals
            printf("warning: ignoring unknown signal: %d", signo);
        } else {
            // capture available signals
            if (signo == SIGINT) {
                printf("handling signal: SIGINT");
                sigint_handler(signo);
            } else if (signo == SIGHUP) {
                printf("handling signal: SIGHUP");
                sighup_handler(signo);
            } else if (signo == SIGQUIT) {
                printf("handling signal: SIGQUIT");
                sigquit_handler(signo);
            } else if (signo == SIGTERM) {
                printf("handling signal: SIGTERM");
                sigterm_handler(signo);
            } else if (signo == SIGTTIN) {
                printf("handling signal: SIGTTIN");
                sigttin_handler(signo);
            } else if (signo == SIGTTOU) {
                printf("handling signal: SIGTTOU");
                sigttou_handler(signo);
            } else if (signo == SIGUSR1) {
                printf("handling signal: SIGUSR1");
                sigusr1_handler(signo);
            } else if (signo == SIGUSR2) {
                printf("handling signal: SIGUSR2");
                sigusr2_handler(signo);
            } else if (signo == SIGWINCH) {
                printf("handling signal: SIGWINCH");
                sigwinch_handler(signo);
            }
            wakeup_master();
        }

    }

    return 0;
}
