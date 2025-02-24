#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int mutex_fd;
    int pid;

    if (argc < 2) {
        fprintf(2, "Usage: mutex_test <string>\n");
        exit(1);
    }

    mutex_fd = cmutex();
    if (mutex_fd < 0) {
        fprintf(2, "cmutex failed\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    char *str = argv[1];
    
    if (pid == 0) { // Child process
        for(int i = 0; str[i] != '\0'; i++) {
            lock(mutex_fd);
            // Добавим небольшую задержку, чтобы увеличить шанс перемешивания
            for(volatile int j = 0; j < 100; j++) { }
            printf("Child(%d): ", getpid());
            write(1, &str[i], 1);
            printf("\n");
            unlock(mutex_fd);
        }
        rmutex(mutex_fd);
        exit(0);
    } else { // Parent process
        for(int i = 0; str[i] != '\0'; i++) {
            lock(mutex_fd);
            // Добавим небольшую задержку, чтобы увеличить шанс перемешивания
            for(volatile int j = 0; j < 100; j++) { }
            printf("Parent(%d): ", getpid());
            write(1, &str[i], 1);
            printf("\n");
            unlock(mutex_fd);
        }
        wait(0);
        rmutex(mutex_fd);
        exit(0);
    }
}
