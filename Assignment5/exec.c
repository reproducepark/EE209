#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "exec.h"
#include "lexsyn.h"
#include "util.h"
#include "dynarray.h"

#define READ 0
#define WRITE 1

typedef char** StringArray; 
/*--------------------------------------------------------------------*/
/*
get token and process it
we already checked if it is lexically, syntactically correct
*/
// typedef 필요
// execute 를 ish 안 함수로 옮기자.
// 해야할일 signal handling
// free와 관련된 부분 -> 함수로 만들자.
// single command와 pipe command를 분리하는 방법도 있다.
// lim ver와 다른 구성으로 바꾸자. 비교필요
// runcmd.c 로 바꾸자
// 그냥 넣자 ish에

void freeArray(StringArray *arr, int num){
    for (int i = 0; i < num; i++){
        free(arr[i]);
    }
    free(arr);
}

void exec(DynArray_T oTokens) {
    enum BuiltinType btype;
    int num_pipe = countPipe(oTokens);
    int num_tokens = DynArray_getLength(oTokens);
    StringArray *argv_arr; 
    int *num_args = (int*)calloc(num_pipe+1, sizeof(int));
    int old_stdin = dup(0);
    int old_stdout = dup(1);

    
    // 이걸 함수로 빼자.
    // allocate memory for each command
    // we will store the arguments for each command in argv_arr
    // argv_arr is array of StringArray
    argv_arr = (StringArray *)calloc(num_pipe + 1, sizeof(StringArray));

    if (argv_arr == NULL){
        errorPrint("Cannot allocate memory", FPRINTF);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_pipe + 1 ; i++){
        argv_arr[i] = (StringArray)calloc(num_tokens+1, sizeof(char*));
        if (argv_arr[i] == NULL){
            errorPrint("Cannot allocate memory", FPRINTF);
            // free memory
            freeArray(argv_arr, i);
            exit(EXIT_FAILURE);
        }
    }



    // we need to set up the arguments
    // 이걸 함수로 빼자
    
    // for 문 안으로 pipe_index를 넣자.
    int pipe_index = 0;
    for (int i = 0; i < num_tokens ; i++){
        struct Token *t = DynArray_get(oTokens, i);
        // 이거 순서도 변경 필요
        if (t->eType == TOKEN_PIPE){
            pipe_index++;
        }
        else if (t->eType == TOKEN_WORD){
            argv_arr[pipe_index][num_args[pipe_index]] = t->pcValue;
            num_args[pipe_index]++;
        }
        else if (t->eType == TOKEN_REDIN && pipe_index == 0){
            FILE* fp = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "r");
            if (fp == NULL){
                freeArray(argv_arr, num_pipe + 1);
                errorPrint("Cannot open file", FPRINTF);
                exit(EXIT_FAILURE);
            }
            dup2(fileno(fp), 0);
            fclose(fp);
            i+=2;
        }
        else if (t->eType == TOKEN_REDOUT && pipe_index == num_pipe){
            FILE* fp = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "w");
            if (fp == NULL){
                freeArray(argv_arr, num_pipe + 1);
                errorPrint("Cannot open file", FPRINTF);
                exit(EXIT_FAILURE);
            }
            dup2(fileno(fp), 1);
            fclose(fp);
            i+=2;
        }
    }
    // dumpArgv(argv_arr, num_pipe + 1, num_args);

    // check if it is a builtin command
    btype = checkBuiltin(DynArray_get(oTokens, 0));
    if (btype == NORMAL){
        if (num_pipe > 0){
            // printf("num_pipe: %d\n", num_pipe);
            // we need to pipe
            int fd[2], fdsave;
            pid_t pid;

            for (int i = 0; i < num_pipe + 1; i++){
                if (i < num_pipe){
                    if (pipe(fd) == -1){
                        freeArray(argv_arr, num_pipe + 1);
                        errorPrint("Cannot create pipe", FPRINTF);
                        exit(EXIT_FAILURE);
                    }
                }
                // printf("fd[0]: %d, fd[1]: %d\n", fd[0], fd[1]);
                pid = fork();
                if (pid < 0)
                {
                    freeArray(argv_arr, num_pipe + 1);
                    errorPrint("Cannot fork", FPRINTF);
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0){

                    // child process
                    // first command
                    if (i == 0){
                        // we need to redirect stdout to fd[1]
                        close(fd[READ]);
                        dup2(fd[WRITE], 1);
                        
                        // we need to close unused fd
                        close(fd[WRITE]);
                    }
                    // last command
                    else if (i == num_pipe){
                        // we need to redirect stdin to fdsave
                        // close(fd[WRITE]);
                        dup2(fdsave, 0);
                        
                        // fprintf(stderr, "fdsave last: %d\n", fdsave);
                        // we need to close unused fd
                        close(fdsave);
                    }
                    // middle command
                    else{
                        
                        close(fd[READ]);
                        // we need to redirect stdin to fdsave
                        dup2(fdsave, 0);

                        // we need to close unused fd
                        close(fdsave);
                        // fprintf(stderr, "fdsave mid: %d\n", fdsave);
                        // we need to redirect stdout to fd[1]
                        dup2(fd[WRITE], 1);
                           
                        
                        // we need to close unused fd
                        close(fd[WRITE]);

                    }
                    // // close all pipes
                    // for (int j = 0; j < num_pipe + 1; j++){
                    //     close(fd[j]);
                    // }
                    // execute command
                    execvp(argv_arr[i][0], argv_arr[i]);

                    // if execvp fails
                    errorPrint(argv_arr[i][0], PERROR);
                    exit(EXIT_FAILURE);
                }
                else{
                    // parent process

                    // wait for child process to finish
                    // fprintf(stderr,"fdsave prnt: %d\n", fdsave);
                    if(i < num_pipe){
                        close(fd[WRITE]);
                        fdsave = dup(fd[READ]);
                        // we need to close unused fd
                        close(fd[READ]);
                    }
                    wait(NULL);
                }
            }
            // restore stdin and stdout
            dup2(old_stdin, 0);
            dup2(old_stdout, 1);
        }
        // just a one command
        else{
            // printf("one command\n");
            // fork a child process and pass the file name to the child
            // the child process will execute the file
            // the parent process will wait for the child process to finish
            // if the child process is terminated by a signal, the parent process
            // will print the signal number
            int pid = fork();
            if (pid < 0){
                errorPrint("Cannot fork", FPRINTF);
                exit(EXIT_FAILURE);
            }
            else if (pid == 0){
                // child process
                execvp(argv_arr[0][0], argv_arr[0]);
                // if execvp fails
                // freeArray(argv_arr, 1);
                errorPrint(argv_arr[0][0], PERROR);
                exit(EXIT_FAILURE);
            }
            else{
                // parent process
                // wait for child process to finish
                wait(NULL);
                // restore stdin and stdout
                dup2(old_stdin, 0);
                dup2(old_stdout, 1);

            }
        }
    }
    else{
        // it is a builtin command
        // we need to execute the builtin command
        exec_builtin(oTokens, num_tokens, btype);

    }

}

// 다른 파일로 빼자.
void exec_builtin(DynArray_T oTokens, int num_tokens, enum BuiltinType btype){
    // if it is exit
    if (btype == B_EXIT){
        // exit the shell
        if (num_tokens == 1){
            printf("\n");
            exit(EXIT_SUCCESS);
        }
        else{
            errorPrint("exit does not take any parameters", FPRINTF);
        }
    }
    // if it is setenv
    else if (btype == B_SETENV){
        // set environment variable
        if (num_tokens == 2){
            struct Token *t = DynArray_get(oTokens, 1);
            char *name = t->pcValue;
            if(setenv(name, "\0", 1) == -1){
                errorPrint("Cannot set environment variable", FPRINTF);
                exit(EXIT_FAILURE);
            }
        }
        else if (num_tokens == 3){
            struct Token *t = DynArray_get(oTokens, 1);
            char *name = t->pcValue;
            t = DynArray_get(oTokens, 2);
            char *value = t->pcValue;
            if(setenv(name, value, 1) == -1){
                errorPrint("Cannot set environment variable", FPRINTF);
                exit(EXIT_FAILURE);
            }
        }
        else{
            errorPrint("setenv takes one or two parameters", FPRINTF);
        }
    }
    // if it is unsetenv
    else if (btype == B_USETENV){
        // unset environment variable
        if (num_tokens == 2){
            struct Token *t = DynArray_get(oTokens, 1);
            char *name = t->pcValue;
            if(unsetenv(name) == -1){
                errorPrint("Cannot unset environment variable", FPRINTF);
                exit(EXIT_FAILURE);
            }
        }
        else{
            errorPrint("unsetenv takes one parameter", FPRINTF);
        }
    }
    // if it is cd
    else if (btype == B_CD){
        // change directory
        if (num_tokens == 1){
            // change to home directory
            if (chdir("HOME") == -1){
            errorPrint("Cannot change directory", FPRINTF);
            }
        }
        else if (num_tokens == 2){
            struct Token *t = DynArray_get(oTokens, 1);
            char *path = t->pcValue;
            // change to the specified directory
            if (chdir(path) == -1){
            errorPrint("No such file or directory", FPRINTF);
            }
        }
        else{
            errorPrint("cd takes one parameter", FPRINTF);
        }

    }
}

