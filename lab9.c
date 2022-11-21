#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdatomic.h>

#define CORRECT_ARGC 2
#define INTERVAL 1e7
#define COUNT_BARRIERS 2


volatile sig_atomic_t end = 0;

typedef struct Params {
    uint64_t num;
    uint64_t *step;
    double result;
    pthread_barrier_t *bar1;
    pthread_barrier_t *bar2;
    int *stop;
}Params;

void handler(int signal) {
    printf("Wait a minute...\n");
    end = 1;
}

void *thread_body(void *arg) {
    Params *param = (Params *)arg;
    uint64_t localInterval = INTERVAL;
    uint64_t i = param->num;
    double localPi = 0.0;
    while(!*param->stop) {
        localPi += 1.0 / (i * 4.0 + 1.0);
        localPi -= 1.0 / (i * 4.0 + 3.0);
        if(i >= localInterval){
            pthread_barrier_wait(param->bar1);
            if (end && !*param->stop) *param->stop = 1;
            pthread_barrier_wait(param->bar2);
            localInterval += INTERVAL;
        }
        i += *param->step;
    }
    
    param->result = localPi;
    
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    void *code_sig = signal(SIGINT, handler);
    if(code_sig == SIG_ERR) {
    	perror("signal(): ");
        return EXIT_FAILURE;
    }

    if(argc != CORRECT_ARGC) {
        fprintf(stderr, "Incorrect arguments\n");
        return EXIT_FAILURE;
    }
    
    uint64_t numThreads = atoll(argv[1]);
    if(numThreads < 1) {
    	fprintf(stderr, "Incorrect number of threads\n");
        return EXIT_FAILURE;
    }
    
    int stop = 0;
    pthread_barrier_t barrier[COUNT_BARRIERS];
    pthread_barrier_init(&barrier[0], NULL, numThreads);
    pthread_barrier_init(&barrier[1], NULL, numThreads);

    int code;
    pthread_t threads[numThreads];
    Params param[numThreads];
    for(uint64_t i = 0; i < numThreads; i++) {
        param[i].num = i;
        param[i].step = &numThreads;
        param[i].bar1 = &barrier[0];
        param[i].bar2 = &barrier[1];
        param[i].stop = &stop;
        code = pthread_create(&threads[i], NULL, thread_body, &param[i]);
        if(code != 0){
            perror("pthread_cancel(): ");
            pthread_barrier_destroy(&barrier[0]);
            pthread_barrier_destroy(&barrier[1]);
            return EXIT_FAILURE;
        }
    }

    double pi = 0;
    for(uint64_t i = 0; i < numThreads; i++) {
        code = pthread_join(threads[i], NULL);
        if(code != 0){
            perror("pthread_cancel(): ");
            pthread_barrier_destroy(&barrier[0]);
            pthread_barrier_destroy(&barrier[1]);
            return EXIT_FAILURE;
        }
        pi += param[i].result;
    }
    
    pi *= 4.0;
    printf ("pi = %.15f\n", pi);
    
    pthread_barrier_destroy(&barrier[0]);
    pthread_barrier_destroy(&barrier[1]);

    return EXIT_SUCCESS;
}
