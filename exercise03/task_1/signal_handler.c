// https://stackoverflow.com/questions/48332332/what-does-define-posix-source-mean
// Includes POSIX and XPG things. 700 for the seventh revision.
#define _XOPEN_SOURCE 700

#include <time.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define true 1

void termination_handler(int signum);
void change_handler(int signum);

void change_handler(int signum) {
    //functions that are asynchronous-signal-safe:
    //sigaction(), sigemptyset()
    if (signum == SIGUSR1 || signum == SIGUSR2 || signum == SIGINT) {
        struct sigaction new_action;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;
        switch (signum) {
            case SIGUSR1:
                new_action.sa_handler = SIG_IGN;
                sigaction(SIGUSR2, &new_action, NULL);
                break;
            case SIGUSR2:
                new_action.sa_handler = SIG_IGN;
                sigaction(SIGUSR1, &new_action, NULL);
                break;
            case SIGINT:
                new_action.sa_handler = termination_handler;
                sigaction(SIGUSR1, &new_action, NULL);
                sigaction(SIGUSR2, &new_action, NULL);
        }
    }
}

void termination_handler(int signum) {
    //https://wiki.sei.cmu.edu/confluence/display/c/SIG34-C.+Do+not+call+signal%28%29+from+within+interruptible+signal+handlers
    //Change the signals from a safe function
    change_handler(signum);
    //Not asyncronos safe!!:
    //FILE *f = fopen("reason.txt", "w");
    //fprintf(f, "%d", signum);
    //fclose(f);
    //printf("%d", signum);
    //fflush(stdout);
    //exit(signum);
}

int main(void) {
    // https://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
    struct sigaction new_action, old_action;

    // Set up the structure to specify the new action.
    new_action.sa_handler = termination_handler;
    // Clear all signals from SET.
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    // Checking against SIGIGN is necessary because you can ignore signals when running in shell intentionally
    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGINT, &new_action, NULL);
    sigaction(SIGSTOP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGSTOP, &new_action, NULL);
    sigaction(SIGCONT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGCONT, &new_action, NULL);
    sigaction(SIGKILL, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGKILL, &new_action, NULL);
    sigaction (SIGUSR1, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGUSR1, &new_action, NULL);
    sigaction(SIGUSR2, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGUSR2, &new_action, NULL);

    const time_t work_seconds = 1;

    struct timespec work_duration = {.tv_sec = work_seconds};

    struct timespec remaining = {0};

    while (true) {
        // simulate real workload
        if (nanosleep(&work_duration, &remaining) == -1 && errno == EINTR) {
            work_duration = remaining;
            continue;
        }

        // restore work_duration
        work_duration.tv_sec = work_seconds;

        // TODO: more code (only if needed)
    }

    return EXIT_SUCCESS;
}