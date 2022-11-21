#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define ITERATIONS_NUMBER 10

void *thread_body(void *arg) {
    for(int i = 0; i < ITERATIONS_NUMBER; i++) {
        printf("Child\n");
    }
    
    pthread_exit(0);
}   

int main() {
    pthread_t thread;
    
    int code = pthread_create(&thread, NULL, thread_body, NULL);
    if (code != 0) {
        perror("pthread_create(): ");
        return EXIT_FAILURE;
    }

    pthread_join(thread, NULL);

    for(int i = 0; i < ITERATIONS_NUMBER; i++) {
        printf("Parent\n");
    }
    
    return EXIT_SUCCESS;
}
