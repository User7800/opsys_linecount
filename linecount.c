#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int total_lines = 0;
pthread_mutex_t mutex;

void* count_lines(void* filename) {
    FILE* fp = fopen((char*) filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buf[2048];
    int *count = (int*) malloc(sizeof(int));
    *count = 0;

    while (fgets(buf, 2048, fp) != NULL) {
        (*count) += 1;
    }
    
    fclose(fp);

    pthread_mutex_lock(&mutex);
    total_lines += (*count);
    pthread_mutex_unlock(&mutex);


    pthread_exit(count);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 file2 ...\n", &argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_files = argc - 1;
    pthread_t threads[num_files];

    for(int i = 0; i < num_files; i += 1) {
        if(pthread_create(&threads[i], NULL, count_lines, (void *) argv[i + 1])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_files; i += 1) {
        int *return_val;
        pthread_join(threads[i], (void**) &return_val);

        printf("%d %s\n", *return_val, argv[i + 1]);

        free(return_val);
    }

    printf("Total lines: %d\n", total_lines);

    exit(EXIT_SUCCESS);
}