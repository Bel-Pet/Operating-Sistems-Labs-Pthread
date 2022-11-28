#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>


#define LINES_COUNT 10
#define CHILD_INDEX 0
#define FIRST "first"
#define SECOND "second"



void printLines(sem_t *sem1, sem_t *sem2, char *str) {
	for (int i = 0; i < LINES_COUNT; i++) {
		sem_wait(sem1);
		printf("%s\n", str);
		sem_post(sem2);
	}
}

int main(int argc, char* argv[]) {

	sem_t *sem1, *sem2;
    sem1 = sem_open(FIRST, O_CREAT | O_EXCL, 0600, 1);
    if (sem1 == SEM_FAILED) {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    sem2 = sem_open(SECOND, O_CREAT | O_EXCL, 0600, 0);
    if (sem1 == SEM_FAILED) {
        perror("sem_open");
        sem_close(sem1);
        sem_unlink(FIRST);
        return EXIT_FAILURE;
    }

    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    
    if (child == CHILD_INDEX) {
        char* str = "Child";
	    printLines(sem2, sem1, str);

        return EXIT_SUCCESS;
    }

    char* str = "Parent";
    printLines(sem1, sem2, str);

    pid_t check = wait(NULL);
    if (check == -1) {
        sem_close(sem1);
        sem_close(sem2);
        sem_unlink(FIRST);
        sem_unlink(SECOND);
        return EXIT_FAILURE;
    }

    sem_close(sem1);
    sem_close(sem2);
    sem_unlink(FIRST);
    sem_unlink(SECOND);
	
	return EXIT_SUCCESS;
}

