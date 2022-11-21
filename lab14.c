#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#define LINES_COUNT 10
#define SEMS_COUNT 2
#define PARENT_INDEX 0
#define CHILD_INDEX 1

typedef struct ArrSems {
    sem_t *sems;
} ArrSems;

void printLines(char* str, ArrSems *arg, int index) {
	for (int i = 0; i < LINES_COUNT; i++) {
		sem_wait(&arg->sems[index]);
		printf("%s\n", str);
		sem_post(&arg->sems[(index + 1) % SEMS_COUNT]);
	}
}

void *thread_body(void* arg) {
    ArrSems *param = (ArrSems *)arg;
    
    char* str = "Child";
	printLines(str, param, CHILD_INDEX);
	
	pthread_exit(NULL);
}

ArrSems *initArrSems() {
	ArrSems *arr = (ArrSems *)malloc(sizeof(ArrSems));
	if (!arr) {
		perror("(ArrSems *)malloc(): ");
		return NULL;
	}

	arr->sems = (sem_t *)malloc(sizeof(sem_t) * SEMS_COUNT);
	if (!arr->sems) {
		perror("(ArrSems *)malloc(): ");
		free(arr);
		return NULL;
	}

	int code = sem_init(&arr->sems[PARENT_INDEX], 0, 1);
	if (code != 0) {
		perror("sem_init: ");
		free(arr->sems);
		free(arr);
		return NULL;
	}

	code = sem_init(&arr->sems[CHILD_INDEX], 0, 0);
	if (code != 0) {
		perror("sem_init: ");
		free(arr->sems);
		free(arr);
		return NULL;
	}
	
	return arr;
}

void destroyArrSems(ArrSems *arg) {
	sem_destroy(&arg->sems[PARENT_INDEX]);
	sem_destroy(&arg->sems[CHILD_INDEX]);
	free(arg->sems);
	free(arg);
}

int main(int argc, char* argv[]) {
	ArrSems *param = initArrSems();
	if (!param) return EXIT_FAILURE;

	pthread_t thread;
	int code = pthread_create(&thread, NULL, thread_body, param);
	if (code != 0) {
        perror("pthread_create(): ");
		destroyArrSems(param);
        return EXIT_FAILURE;
	}

	char* str = "Parent";
	printLines(str, param, PARENT_INDEX);

	code = pthread_join(thread, NULL);
    if (code != 0) {
        perror("pthread_join(): ");
		destroyArrSems(param);
        return EXIT_FAILURE;
    }
	
	return EXIT_SUCCESS;
}

