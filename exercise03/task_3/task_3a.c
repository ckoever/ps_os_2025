#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int64_t accumulation;

void *accumulate_func(void *param) {
    int num = *((int *)param);
    for (; num > 0; num--, accumulation+=num);
    pthread_exit(0);
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("Usage: %s [N::int]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int num = atoi(argv[1]);

    printf("Begin fork: %ld\n", accumulation);

    pid_t pid_t = fork();
    if (pid_t < 0) {
        fprintf(stderr, "Could not create fork\n");
        exit(EXIT_FAILURE);
    }
    //Parent
    else if (pid_t == 0) {
        //Wait for every fork to finish!
        wait(NULL);
        printf("End fork: %ld\n", accumulation);
    }
    //Child
    else {
        for (; num > 0; num--, accumulation+=num);
        exit(EXIT_SUCCESS);
    }

    printf("Begin pthread: %ld\n", accumulation);

    pthread_t pid_p;
    pthread_attr_t attr_p;
    pthread_attr_init(&attr_p);
    pthread_create(&pid_p, &attr_p, accumulate_func, (void *) &num);
    pthread_join(pid_p, NULL);
    //This returns the correct value, the reason is that POSIX Threads share the global Memory with all including main.
    printf("End pthread: %ld\n", accumulation);
}