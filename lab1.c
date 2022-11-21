#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define ITERATIONS_NUMBER 10

void *thread_body(void *arg) {
    for(int i = 0; i < ITERATIONS_NUMBER; i++) {
        printf("Child\n");
        usleep(500000);
    }

    pthread_exit(EXIT_SUCCESS);
}

int main() {
    pthread_t thread;

    int code = pthread_create(&thread, NULL, thread_body, NULL);
    if (code != 0) {
        perror("pthread_create(): ");
        return EXIT_FAILURE;
    }
    

    for(int i = 0; i < ITERATIONS_NUMBER; i++) {
        printf("Parent\n");
        sleep(1);
    }
   
    pthread_exit(EXIT_SUCCESS);
}
