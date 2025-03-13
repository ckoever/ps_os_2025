#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

double DR_p(const int t, const int n, const unsigned long long s);

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: %s [SIDES] [TRIES]\n", *argv);
        return EXIT_FAILURE;
    }

    int sides = atoi(*(++argv));
    unsigned long long tries = strtoull(*(++argv), NULL, 10);

    size_t ach_sum = 2 * sides;
    int pid[ach_sum - 1];

    for (size_t cur = 0; cur < ach_sum - 1; cur++) {
        pid[cur] = fork();
        if (pid[cur] == 0) {
            clock_t start = clock();
            double result = DR_p(cur + 2, sides, tries);
            clock_t end = clock();
            double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
            printf("Child %ld PID=%d, DR_p(%ld, %d, %llu)=%lf, Time used=%f\n",cur, getpid(), cur + 2, sides, tries, result, elapsed);
            return EXIT_SUCCESS;
        }
    }

    for (size_t cur = 0; cur < ach_sum - 1; cur++) {
        waitpid(pid[cur], NULL, 0);
    }

    printf("Done.\n");

    return EXIT_SUCCESS;
}

double DR_p(const int t, const int n, const unsigned long long s) {
    unsigned long long h = 0;
    srand(getpid());
    for (unsigned long long i = s; i--;) {
        h += (rand() % n + rand() % n + 2 == t);
    }
    return (double)h / s;
  }