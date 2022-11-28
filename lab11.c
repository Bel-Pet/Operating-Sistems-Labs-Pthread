#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LINES_COUNT 10
#define MUTEX_COUNT 3
#define PARENT_INDEX 0
#define CHILD_INDEX 1
#define THREADS_COUNT 2

typedef struct Param {
    // pthread_barrier_t bar;
    pthread_mutex_t *mutex;
    int count;
} Param;

void printLines(char* str, Param *arg, int index) {
	for (int i = 0; i < LINES_COUNT; i++) {
	    //pthread_barrier_wait(&arg->bar);
        pthread_mutex_lock(&(arg->mutex)[(2 * i + index) % arg->count]);

		printf("%s\n", str);

        pthread_mutex_unlock(&(arg->mutex)[(2 * i + 1 + index) % arg->count]);
	}
}

void *thread_body(void* arg) {
    Param *param = (Param *)arg;
    pthread_mutex_lock(&(param->mutex)[CHILD_INDEX + 1]);
    
    char* str = "Child";
	printLines(str, param, CHILD_INDEX);
	
	pthread_exit(NULL);
}

void destroyParam(Param *arg, int count) {
    for (int i = 0; i < count; i++){
        pthread_mutex_destroy(&arg->mutex[i]);
    }
    // pthread_barrier_destroy(&arg->bar);
    free(arg->mutex);
}

int initParam(Param *arg, int count) {
    // int code = pthread_barrier_init(&arg->bar, NULL, THREADS_COUNT);
    // if (code != 0) {
    //     perror("pthread_barrier_init(): ");
    //     return EXIT_FAILURE;
    // }

    arg->count = MUTEX_COUNT;
    arg->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * count);
    if(!arg->mutex) {
        perror("malloc(): ");
        // pthread_barrier_destroy(&arg->bar);
        return EXIT_FAILURE;
    }

    pthread_mutexattr_t attr;
	int code = pthread_mutexattr_init(&attr);
    if(code != 0) {
        perror("pthread_mutexattr_init(): ");
        // pthread_barrier_destroy(&arg->bar);
        free(arg->mutex);
        return EXIT_FAILURE;
    }

	code = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if(code != 0) {
        perror("pthread_mutexattr_settype(): ");
        // pthread_barrier_destroy(&arg->bar);
        free(arg->mutex);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < count; i++){
		code = pthread_mutex_init(&arg->mutex[i], &attr);
		if (code != 0) {
            perror("pthread_mutex_init(): ");
            for (int j = 0; j < i; j++) {
                pthread_mutex_destroy(&arg->mutex[j]);
            }
            pthread_mutexattr_destroy(&attr);
            // pthread_barrier_destroy(&arg->bar);
            free(arg->mutex);
            return EXIT_FAILURE;
	    }
	}
    pthread_mutexattr_destroy(&attr);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    Param param;
    int code = initParam(&param, MUTEX_COUNT);
    if (code == EXIT_FAILURE) return EXIT_FAILURE;
	
    pthread_mutex_lock(&param.mutex[PARENT_INDEX + 1]);

    pthread_t thread;
    code = pthread_create(&thread, NULL, thread_body, &param);
	if (code != 0) {
        perror("pthread_create(): ");
        destroyParam(&param, MUTEX_COUNT);
        return EXIT_FAILURE;
	}
    
    sleep(1);

    char* str = "Parent";
	printLines(str, &param, PARENT_INDEX);

	code = pthread_join(thread, NULL);
    if (code != 0) {
        perror("pthread_join(): ");
        destroyParam(&param, MUTEX_COUNT);
        return EXIT_FAILURE;
    }

    destroyParam(&param, MUTEX_COUNT);

	return EXIT_SUCCESS;
}
