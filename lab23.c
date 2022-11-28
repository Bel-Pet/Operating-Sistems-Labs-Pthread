#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

#define BUF_SIZE 256


typedef struct ThreadList {
        char *str;
        pthread_t thread;
        struct ThreadList *next;
}ThreadList;

void *thread_body(void *arg) {
    char *str = (char *)arg;
    usleep(10000 * strlen(str));
    printf("%s", str);
    
    pthread_exit(NULL);
}

int creatThreadList(ThreadList *list) {
    list->str = (char *)malloc(BUF_SIZE * sizeof(char));
    if(!list->str) {
    	perror("list->str: malloc() ");
    	return EXIT_FAILURE;
    }
    
    list->next = NULL;
    
    return EXIT_SUCCESS;
}

int readLines(ThreadList *list) {
    if(feof(stdin)) return EXIT_SUCCESS;
    
    int code = creatThreadList(list);
    if(code == EXIT_FAILURE) {
    	return EXIT_FAILURE;
    }
    
    char *res_fgets = fgets(list->str, BUF_SIZE, stdin);
    if(!res_fgets) {
    	if(feof(stdin)) return EXIT_SUCCESS;
        perror("fgets(): ");
    	return EXIT_FAILURE;
    }
    
    code = pthread_create(&(list->thread), NULL, thread_body, list->str);
    if(code != 0) {
        perror("pthread_create(): ");
        return EXIT_FAILURE;
    }
    
    list->next = (ThreadList *)malloc(sizeof(ThreadList));
    if(!list->next) {
        perror("list->next: malloc() ");
        return EXIT_FAILURE;
    }
    
    return readLines(list->next);
}

void cleanThreadList(ThreadList *list) {
    if(!list) return;
    
    if(!list->str) return;
    pthread_join(list->thread, NULL);
    free(list->str);
    
    if(!list->next) return;
    cleanThreadList(list->next);
    free(list->next);
}

int main(int argc, char *argv[]) {
    ThreadList *list = (ThreadList *)malloc(sizeof(ThreadList));
    if(!list) {
        perror("list->next: malloc() ");
        return EXIT_FAILURE;
    }
    
    int result = readLines(list);
    if(result == EXIT_FAILURE) {
    	cleanThreadList(list);
    	fprintf(stderr, "Fail\n");
    	return EXIT_FAILURE;
    }
    
    cleanThreadList(list);

    return EXIT_SUCCESS;
}

