#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

bool test_number(int number){
    if (number < 0){
        return true;
    }
    for (int i = 2; i <= number/2; i++){
        if (number % i == 0){
            return false;
        }
    }
    return true;
}

int main(){
    int pid = 0;
    int fd1[2];
    int fd2[2];

    char filepath[PATH_MAX];
    scanf("%s", filepath);
    FILE* file;
    file = fopen(filepath, "w");

    if (pipe(fd1) == -1)
        return 1;
    
    if (pipe(fd2) == -1)
        return 1;

    pid = fork();
    if (pid == -1)
        return 2;

    int number_to_test;
    int status = 0;

    if (pid == 0){
        while (read(fd1[0], &number_to_test, sizeof(number_to_test))){
            if (test_number(number_to_test)){
                status = 1;
                write(fd2[1], &status, sizeof(status));
                break;
            }
            else {
                status = 0;
                fprintf(file, "%d\n", number_to_test);
                write(fd2[1], &status, sizeof(status));
            }
        }
    }
    else {
        while (status == 0 && scanf("%d", &number_to_test) != EOF){
            write(fd1[1], &number_to_test, sizeof(number_to_test));
            read(fd2[0], &status, sizeof(status));
        }
        if (status == 0){
            number_to_test = -1;
            write(fd1[1], &number_to_test, sizeof(number_to_test));
        }
    }
    wait(NULL);

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    fclose(file);
    return 0;
}