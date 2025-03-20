#define _XOPEN_SOURCE 700

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int id;
    const char *file;
    int sum;
    int err;
} us_file_proc;

void *process_file(void *attr) {
    us_file_proc *cur = ((us_file_proc*)attr);
    us_file_proc *ret = calloc(1, sizeof(*ret));
    ret->id = cur->id;
    ret->err = 0;
    FILE *cur_file = fopen(cur->file, "r");
    if (cur_file == NULL) {
        fprintf(stderr, "Could not open file %s\n", cur->file);
        ret->err = 1;
        return ret;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, cur_file)) != -1) {
        ret->sum += atoi(line);
    }
    free(line);
    fclose(cur_file);
    return ret;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("Usage: %s {files::char[]}\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char **files = argv + 1;
    int file_count = argc - 1;

    pthread_t pid_p[file_count];
    pthread_attr_t attr_p;
    us_file_proc **param_p = malloc(file_count * sizeof(*param_p));
    for (int i = 0; i < file_count; i++) {
        param_p[i] = malloc(file_count * sizeof(*param_p[i]));
        param_p[i]->id = i + 1;
        param_p[i]->file = files[i];
        if (pthread_attr_init(&attr_p)) {
            fprintf(stderr, "Error creating pthread attr\n");
        }
        else if (pthread_create(&pid_p[i], &attr_p, process_file, (void *) param_p[i])) {
            fprintf(stderr, "Error creating pthread\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Destroy the thread attributes object, since it is not longer needed. */
    if (pthread_attr_destroy(&attr_p)) {
        fprintf(stderr, "Error deliting pthread attr\n");
    }

    void *res;
    int total = 0;
    us_file_proc *res_param_p;
    for (int i = 0; i < file_count; i++) {
        if (pthread_join(pid_p[i], &res)) {
            fprintf(stderr, "Error joinig pthread\n");
            continue;
        }
        res_param_p = (us_file_proc*) res;
        if (res_param_p->err) {
            fprintf(stderr, "Error getting pthread result\n");
            continue;
        }
        total += res_param_p->sum;
        printf("sum %d = %d\n", res_param_p->id, res_param_p->sum);
        free(res);
    }
    printf("total = %d\n", total);

    for(int i = 0; i < file_count; i++) {
        free(param_p[i]);
    }
    free(param_p);

    exit(EXIT_SUCCESS);
}