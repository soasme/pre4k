#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>

void wakeup_master();

int accept_requests = 0;
int num_workers = 2;
int graceful_timeout = 20;
int heartbeat_interval = 5;
int heartbeat_timeout = 20;
const char* pidfile = "/tmp/prefork.pid";
int pipe_fd[2];
char pipe_buf[2];

typedef struct _queue {
    int length;
    int current;
    int queue[5];
} Queue ;

Queue queue = {
    .length = 5,
    .current = -1,
};

void
debug_queue(Queue queue)
{
    printf("debug: queue current %d, values: ", queue.current);
    for (int i = 0; i < queue.length; i++) {
        printf("%d ", queue.queue[i]);
    }
    printf("\n");
}

int
push_queue(Queue queue, int elem)
{
    queue.current += 1;
    queue.current %= queue.length;
    queue.queue[queue.current] = elem;
    debug_queue(queue);
    return 0;
}

int
pop_queue(Queue queue) {
    if (queue.current < 0) {
        printf("error: pre4k can't pop element from empty queue");
        exit(1);
    }
    int elem = queue.queue[queue.current];
    queue.current -= 1;
    return elem;
}

int
count_queue(Queue queue) {
    return queue.current;
}

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
pre4k_buff_signals(int signo) {
    printf("info: receive signal: %d\n", signo);
    push_queue(queue, signo);
    wakeup_master();
}

void
register_signals()
{
    int signals[9] = {
        SIGHUP, SIGQUIT, SIGINT, SIGTERM,
        SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2,
        SIGWINCH
    };
    for (int i = 0; i < 9; i++) {
        if (signal(signals[i], pre4k_buff_signals) == SIG_ERR) {
            printf("register signals for %d failed.\n", signals[i]);
            exit(1);
        }
    }
}


void
start()
{
    // fixme: create pidfile

    register_signals();
    /*if (signal(SIGTTIN, sigttin_handler) == SIG_ERR) {
        printf("error: cannot receive sigttin.\n");
        exit(1);
    }*/
}

void
manage_workers()
{

}

int
maybe_get_signo()
{
    if (count_queue(queue) == -1) {
        return 0;
    } else {
        int signo = pop_queue(queue);
        return signo;
    }
}

void
sleep_master()
{
    fd_set rset;
    struct timeval timeout;

    FD_ZERO(&rset);
    FD_SET(pipe_fd[0], &rset);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ready = select(FD_SETSIZE, &rset, NULL, NULL, &timeout);
    if (ready <= 0) {
        printf("debug: sleeping...\n");
        return;
    } else {
        // fixme: EAGAIN, EINTR
        if (read(pipe_fd[0], &pipe_buf, 1) != 1) {
            printf("error: unexpected read: %s", pipe_buf);
            exit(1);
        }
    }

}

void
wakeup_master()
{
    // fixme: EAGAIN, EINTR
    write(pipe_fd[1], ".", 1);
    printf("debug: pre4k wrote . to pipe.\n");
}

void
murder_workers()
{

}

void
set_pipe(int fd) {
    // set non blocking
    int flags = fcntl(fd, F_GETFL);
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

    printf("starting pre4k\n");

    pipe(pipe_fd);
    set_pipe(pipe_fd[0]);
    set_pipe(pipe_fd[1]);

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
