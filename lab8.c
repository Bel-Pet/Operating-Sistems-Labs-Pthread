#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define NUM_STEPS 2000000000
#define CORRECT_ARGC 2


typedef struct Params {
    uint64_t num;
    uint64_t *step;
    double result;
}Params;


void *thread_body(void *arg) {
    Params *param = (Params *)arg;
    
    double localPi = 0.0;
    for(uint64_t i = param->num; i < NUM_STEPS ; i += *param->step) {
        localPi += 1.0 / (i * 4.0 + 1.0);
        localPi -= 1.0 / (i * 4.0 + 3.0);
    }
    
    param->result = localPi;
    
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if(argc != CORRECT_ARGC) {
        fprintf(stderr, "Incorrect arguments\n");
        return EXIT_FAILURE;
    }
    
    uint64_t numThreads = atoll(argv[1]);
    if(numThreads < 1) {
    	fprintf(stderr, "Incorrect number of threads\n");
        return EXIT_FAILURE;
    }

    pthread_t threads[numThreads];
    Params param[numThreads];
    exit(0);
    
    int code;
    for(uint64_t i = 0; i < numThreads; i++) {
        param[i].num = i;
        param[i].step = &numThreads;
        
        code = pthread_create(&threads[i], NULL, thread_body, &param[i]);
        if(code != 0){
            perror("pthread_cancel(): ");
            return EXIT_FAILURE;
        }
    }

    double pi = 0;
    
    for(uint64_t i = 0; i < numThreads; i++) {
        code = pthread_join(threads[i], NULL);
        if(code != 0){
            perror("pthread_cancel(): ");
            return EXIT_FAILURE;
        }
        pi += param[i].result;
    }

    pi *= 4.0;
    printf("pi = %.15f\n", pi);

    return EXIT_SUCCESS;
}