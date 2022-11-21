#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define TIME 1

void cancel_handler(void * arg) {
    printf("%s\n", (char*)arg);
}

void* thread_body(void * param) {
    pthread_cleanup_push(cancel_handler, (void *)"I'm cancelled");

    while(true){
        printf("I'm child\n");
    }
    
    pthread_cleanup_pop(0);
}

int main(int argc, char *argv[]) {
    pthread_t thread;
    
    int code = pthread_create(&thread, NULL, thread_body, NULL);
    if(code != 0) {
        perror("pthread_create(): ");
        return EXIT_FAILURE;
    }
    
    sleep(TIME);
    
    code = pthread_cancel(thread);
    if(code != 0) {
        perror("pthread_cancel(): ");
        return EXIT_FAILURE;
    }
   
    void *status;
    code = pthread_join(thread, &status);
    if(code != 0) {
        perror("pthread_join(): ");
        return EXIT_FAILURE;
    }
    
    if(status == PTHREAD_CANCELED) {
    	printf("Thread was cancelled\n");
    }
      
    return EXIT_SUCCESS;
}
