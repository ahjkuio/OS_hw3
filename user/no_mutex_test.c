#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid;

    if (argc < 2) {
        fprintf(2, "Usage: no_mutex_test <string>\n");
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
            // Добавим небольшую задержку, чтобы увеличить шанс перемешивания
            for(volatile int j = 0; j < 100; j++) { }
            printf("Child(%d): ", getpid());
            write(1, &str[i], 1);
            printf("\n");
        }
        exit(0);
    } else { // Parent process
        for(int i = 0; str[i] != '\0'; i++) {
            // Добавим небольшую задержку, чтобы увеличить шанс перемешивания
            for(volatile int j = 0; j < 100; j++) { }
            printf("Parent(%d): ", getpid());
            write(1, &str[i], 1);
            printf("\n");
        }
        wait(0);
        exit(0);
    }
}
