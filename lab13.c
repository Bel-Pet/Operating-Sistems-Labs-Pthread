#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LINES_COUNT 10
#define PARENT_INDEX 0
#define CHILD_INDEX 1

typedef struct Param {
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int *next;
} Param;

void printLines(char* str, Param *arg, int index) {
	for (int i = 0; i < LINES_COUNT; i++) {
		pthread_mutex_lock(arg->mutex);
		while (*arg->next == index) {
			pthread_cond_wait(arg->cond, arg->mutex);
		}
		printf("%s\n", str);
		*arg->next = index;
		pthread_cond_signal(arg->cond);
		pthread_mutex_unlock(arg->mutex);
	}
}

void *thread_body(void* arg) {
    Param *param = (Param *)arg;
    
    char* str = "Child";
	printLines(str, param, PARENT_INDEX);
	
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	Param param;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int next = PARENT_INDEX;

	param.cond = &cond;
	param.mutex = &mutex;
	param.next = &next;

	pthread_t thread;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	int code = pthread_create(&thread, NULL, thread_body, &param);
	if (code != 0) {
        perror("pthread_create(): ");
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
        return EXIT_FAILURE;
	}

	char* str = "Parent";
	printLines(str, &param, CHILD_INDEX);

	code = pthread_join(thread, NULL);
    if (code != 0) {
        perror("pthread_join(): ");
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
        return EXIT_FAILURE;
    }


	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	return EXIT_SUCCESS;
}
