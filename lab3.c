#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define THREADS_NUMBER 4
#define NUMBER_PARAM 16

const char *params[NUMBER_PARAM][NUMBER_PARAM] = {{"1", "2", "3", NULL},
     				 	{"2", "4", "6", NULL},
      				 	{"0", "1", "1", NULL},
       				 	{"5", "10", "8", NULL}};

void *thread_body(void *param) {
    for (char **t = (char **)param; *t != NULL; t++) {
        printf("%s ", *t);
    }
 
    pthread_exit(EXIT_SUCCESS);
}

int main() {
    pthread_t thread[THREADS_NUMBER];	 
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    int code;
    
    for(int i = 0; i < THREADS_NUMBER; i++) {
        code = pthread_create(&thread[i], &attr, thread_body, params[i]);
    	if (code != 0) {
            perror("pthread_create(): ");
            return EXIT_FAILURE;
    	}
    }
    
    pthread_exit(EXIT_SUCCESS);
}
