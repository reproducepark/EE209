#include <stdio.h>
#include <stdlib.h>

#include "lexsyn.h"
#include "util.h"

/*--------------------------------------------------------------------*/
/*
get token and process it
we already checked if it is lexically, syntactically correct
*/

int exec(DynArray_T oTokens) {
    enum BuiltinType btype;

    if (countPipe(oTokens)>0){
        // we need to pipe
    }
    else{
        // we don't need to pipe
        // just one command
        // check first token
        int num_args = 0;
        btype = checkBuiltin(DynArray_get(oTokens, 0));
        char **argv = (char**)calloc(DynArray_getLength(oTokens)+1, sizeof(char*));

        // we need to set up the arguments for execvp
        for(int i = 1; i < DynArray_getLength(oTokens); i++){
            struct Token *t = DynArray_get(oTokens, i);
            if (t->eType == TOKEN_WORD){
                argv[i] = t->pcValue;
                num_args++;
            }
            else if (t->eType == TOKEN_REDIN){
                FILE* fd = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "r");
                if (fd == NULL){
                    errorPrint("Cannot open file", PERROR);
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(fd), 0);
                fclose(fd);
                i+=2;
            }
            else if (t->eType == TOKEN_REDOUT){
                FILE* fd = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "w");
                if (fd == NULL){
                    errorPrint("Cannot open file", PERROR);
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(fd), 1);
                fclose(fd);
                i+=2;
            }
        }
        // if it is not a builtin command
        if (btype == NORMAL){
            // fork a child process and pass the file name to the child
            // the child process will execute the file
            // the parent process will wait for the child process to finish
            // if the child process is terminated by a signal, the parent process
            // will print the signal number
            int pid = fork();
            if (pid == 0){
                // child process
                // execute the file
                struct Token *t = DynArray_get(oTokens, 0);
                char *filename = t->pcValue;
                argv[0] = filename;
                // we already set up the arguments for execvp
                execvp(filename, argv);
                errorPrint("Cannot execute file", PERROR);
                exit(EXIT_FAILURE);
            }
            else if (pid > 0){
                // parent process
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)){
                    // child process terminated normally
                    // do nothing
                }
                else if (WIFSIGNALED(status)){
                    // child process terminated by a signal
                    // print the signal number
                    printf("Terminated by signal %d", WTERMSIG(status));
                }
                else{
                    // fork failed
                    errorPrint("Cannot fork a child process", PERROR);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else{
            // it is a builtin command
            // we need to execute the builtin command
            exec_builtin(oTokens, num_args, argv);
        }
        // if it is exit
        else if (btype == B_EXIT){
            // exit the shell
            if (num_args == 0){
                exit(EXIT_SUCCESS);
            }
            else{
                errorPrint("exit does not take any parameters", FPRINTF);
            }
        }
        // if it is setenv
        else if (btype == B_SETENV){
            // set environment variable
            struct Token *t = DynArray_get(oTokens, 1);
            char *name = t->pcValue;
            t = DynArray_get(oTokens, 2);
            char *value = t->pcValue;
            setenv(name, value, 1);
        }
        // if it is unsetenv
        else if (btype == B_USETENV){
            // unset environment variable
            struct Token *t = DynArray_get(oTokens, 1);
            char *name = t->pcValue;
            unsetenv(name);
        }
        // if it is cd
        else if (btype == B_CD){
            // change directory
            struct Token *t = DynArray_get(oTokens, 1);
            char *path = t->pcValue;

            if (num_args == 0){
                // change to home directory
                chdir(getenv("HOME"));
            }
            else if (num_args == 1){
                // change to the specified directory
                chdir(path);
                if (chdir(path) == -1){
                errorPrint("Cannot change directory", PERROR);
                }
            }
            else{
                errorPrint("cd takes one parameter", PERROR);
            }

            // char *oldpwd = getenv("OLDPWD");
            // setenv("OLDPWD", oldpwd, 1);

        }
        //     if (fd == NULL){
        //         errorPrint("Cannot open file", PERROR);
        //         exit(EXIT_FAILURE);
        //     }
        //     dup2(fileno(fd), 1);
        //     fclose(fd);
        //     i+=2;
        //     }
        // }
        // first, check if it is a builtin command

    
    }
}

int exec_builtin(DynArray_T oTokens, int num_args, enum BuiltinType btype){
    // if it is exit
    if (btype == B_EXIT){
        // exit the shell
        if (num_args == 0){
            exit(EXIT_SUCCESS);
        }
        else{
            errorPrint("exit does not take any parameters", FPRINTF);
        }
    }
    // if it is setenv
    else if (btype == B_SETENV){
        // set environment variable
        struct Token *t = DynArray_get(oTokens, 1);
        char *name = t->pcValue;
        t = DynArray_get(oTokens, 2);
        char *value = t->pcValue;
        setenv(name, value, 1);
    }
    // if it is unsetenv
    else if (btype == B_USETENV){
        // unset environment variable
        struct Token *t = DynArray_get(oTokens, 1);
        char *name = t->pcValue;
        unsetenv(name);
    }
    // if it is cd
    else if (btype == B_CD){
        // change directory
        struct Token *t = DynArray_get(oTokens, 1);
        char *path = t->pcValue;

        if (num_args == 0){
            // change to home directory
            chdir(getenv("HOME"));
        }
        else if (num_args == 1){
            // change to the specified directory
            chdir(path);
            if (chdir(path) == -1){
            errorPrint("Cannot change directory", PERROR);
            }
        }
        else{
            errorPrint("cd takes one parameter", PERROR);
        }

    }

}