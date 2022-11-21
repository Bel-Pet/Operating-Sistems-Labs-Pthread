#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 128

typedef struct LinkedList {
    char *str;
    struct LinkedList *next;
}LinkedList;

typedef struct ThreadList {
    pthread_t thread;
    struct ThreadList *next;
}ThreadList;

LinkedList *resList;
ThreadList *threadList;
pthread_mutex_t mutex;

ThreadList *createNodeThread() {
    ThreadList *list = (ThreadList *)malloc(sizeof(ThreadList));
    if (!list) {
        return NULL;
    }
    
    list->next = NULL;

    return list;
}

LinkedList *createNode() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (!list) {
        perror("(LinkedList *)malloc(): ");
        return NULL;
    }
    list->next = NULL;
    
    return list;
}

int addLinkedList(LinkedList *list, char *str) {
    if (!list->next) return addLinkedList(list->next, str);
    
    list->str = str;

    list->next = createNode();
    if (!list->next) return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

void clearLinkedList(LinkedList *list) {
    if (list->str) free(list->str);
    
    if (!list->next) {
        free(list);
        return;
    }

    return clearLinkedList(list->next);
}

int printLinkedList(LinkedList *list) {
    if (!list) return EXIT_SUCCESS;

    if (!list->str) return EXIT_FAILURE;
    printf("%s\n", list->str);

    return printLinkedList(list->next);
}

void *thread_body(void *arg) {
    char *str = (char *)arg;
    usleep(10000 * strlen(str));

    pthread_mutex_lock(&mutex);
    addLinkedList(resList, str);
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

int readLine(ThreadList *arg) {
    if(feof(stdin)) return EXIT_SUCCESS;

    char *str = (char *)malloc(sizeof(char) * BUF_SIZE);
    if (!str) {
        return EXIT_FAILURE;
    }

    char *res_fgets = fgets(str, BUF_SIZE, stdin);
    if(!res_fgets) {
        if(feof(stdin)) return EXIT_SUCCESS;
        perror("fgets(): ");
        return EXIT_FAILURE;
    }
    
    int code = pthread_create(&arg->thread, NULL, thread_body, str);
    if(code != 0) {
        perror("pthread_create(): ");
        return EXIT_FAILURE;
    }

    arg->next = createNodeThread();

    return readLine(arg->next);
}

void clearThreadList(ThreadList *arg) {
    if(!arg) return;
    pthread_join(arg->thread, NULL);
    clearThreadList(arg->next);
    free(arg);
}

int main() {
    resList = createNode();
    if (!resList) return EXIT_FAILURE;

    threadList = createNodeThread();
    if (!threadList) return EXIT_FAILURE;

    pthread_mutex_init(&mutex, NULL);
    
    int code = readLine(threadList);
    if (code == EXIT_FAILURE) return EXIT_FAILURE;

    clearThreadList(threadList);
    printLinkedList(resList);
    clearLinkedList(resList);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}