//20210251 jaehyun park, EE209 assignment 5, ish.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

#include "lexsyn.h"
#include "util.h"
#include "dynarray.h"

// for pipe
#define READ 0
#define WRITE 1

// typedef for array of strings
typedef char** StringArray;

/*
function : freeArray
Free the memory allocated for nested StringArray

input : pointer to StringArray, number of elements in StringArray
output : none

Since we do not own the memory allocated for the strings in StringArray,
we do not free the memory allocated for the strings.
*/
static void freeArray(StringArray *arr, int num);

/*
function : freeDynArray
Free the memory allocated for DynArray_T. Which is tokens

input : dynamic array of tokens
output : none
*/
static void freeDynArray(DynArray_T oTokens);

/*
function : argProcess
Preprocess the tokens and store the arguments of each command in argv_arr
argProcess also resolves the redirection of standard input and output

input : dynamic array of tokens, pointer to StringArray
output : EXIT_SUCCESS or EXIT_FAILURE
*/
static int argProcess(DynArray_T oTokens, StringArray *argv_arr);

/*
function : exec
Execute the command. If the command is built-in, call execBuiltIn function
It handles the pipelining of commands too.

input : dynamic array of tokens
output : none
*/
static void exec(DynArray_T oTokens);

/*
function : execBuiltIn
Execute the built-in command.

input : dynamic array of tokens, type of built-in command
output : none
*/
static void execBuiltIn(DynArray_T oTokens, enum BuiltinType btype);

/*
function : shellHelper
Helper function for main function. Check the line is syntactically correct
and lexically correct. If it is, execute the command by calling exec function

input : line to be checked
output : none
*/
static void shellHelper(const char *inLine);

/*
function : sigHandler
Handle the signals for SIGQUIT and SIGALRM. Implement the quit feature of ish.
It uses static variable to check if the signal is sent twice in 5 seconds.

input : signal number
output : none
*/
static void sigHandler(int sig);

/*
function : main
Main function of ish.
First, it executes ishrc file in HOME directory. By calling shellHelper function.
It prints each lines of ishrc and execute the lines of ishrc.
Second, it gets the command from user and execute it. By calling shellHelper 
function. It prints "% " and get the command from user.
*/
int main(int argc, char* argv[]) {
  // When ishrc exists in HOME, run it
  // Print each lines of ishrc, run the lines of ishrc by shellHelper function

  // initialize errorPrint
  errorPrint(argv[0],SETUP);

  // setup directories
  char *homeDir = getenv("HOME");
  char ishrcDir[PATH_MAX];
  char workingDir[PATH_MAX];
  getcwd(workingDir, PATH_MAX);
  
  // setup signal set
  sigset_t sSet;
  sigemptyset(&sSet);
  sigaddset(&sSet, SIGINT);
  sigaddset(&sSet, SIGQUIT);
  sigaddset(&sSet, SIGALRM);

  // unblock signals
  sigprocmask(SIG_UNBLOCK, &sSet, NULL);

  // ignore SIGINT
  void (*pfRet) (int);
  pfRet = signal(SIGINT, SIG_IGN);
  assert(pfRet != SIG_ERR);

  pfRet = signal(SIGQUIT, sigHandler);
  assert(pfRet != SIG_ERR);

  pfRet = signal(SIGALRM, sigHandler);
  assert(pfRet != SIG_ERR);

  // change directory to HOME
  chdir(homeDir);
  
  // get ishrc directory
  strcpy(ishrcDir, homeDir);
  strcat(ishrcDir, "/.ishrc");

  // open ishrc file
  FILE *fp = fopen(ishrcDir, "r");

  // if ishrc exists, run it. else, do nothing
  if (fp != NULL) {
    char line[PATH_MAX + 2];
    while (fgets(line, PATH_MAX, fp) != NULL) {
      // print the lines of ishrc
      fprintf(stdout, "%% ");
      fflush(stdout);

      // if line does not end with '\n'
      if (strchr(line,'\n') == NULL) {
        fprintf(stdout, "%s\n", line);
        fflush(stdout);
      }

      // if line ends with '\n'
      else {
        fprintf(stdout, "%s", line);
        fflush(stdout);
      }

      // execute the line
      shellHelper(line);
    }
    fclose(fp);
  }
  
  // change directory to working directory
  chdir(workingDir);

  char acLine[MAX_LINE_SIZE + 2];
  while (1) {
    fprintf(stdout, "%% ");
    fflush(stdout);
    // get the line from user
    // if no input, exit
    if (fgets(acLine, MAX_LINE_SIZE, stdin) == NULL) {
      printf("\n");
      fflush(stdout);
      exit(EXIT_SUCCESS);
    }
    // execute the line
    shellHelper(acLine);
  }
}

static void
shellHelper(const char *inLine) {
  DynArray_T oTokens;

  enum LexResult lexcheck;
  enum SyntaxResult syncheck;

  oTokens = DynArray_new(0);
  if (oTokens == NULL) {
    errorPrint("Cannot allocate memory", FPRINTF);
    exit(EXIT_FAILURE);
  }

  lexcheck = lexLine(inLine, oTokens);
  switch (lexcheck) {
    case LEX_SUCCESS:
      if (DynArray_getLength(oTokens) == 0){
        // free the dynamic array
        DynArray_free(oTokens);
        return;
      }

      /* dump lex result when DEBUG is set */
      dumpLex(oTokens);

      syncheck = syntaxCheck(oTokens);
      if (syncheck == SYN_SUCCESS) {
        /* execute the command */
        exec(oTokens);
      }

      /* syntax error cases */
      else if (syncheck == SYN_FAIL_NOCMD){
        freeDynArray(oTokens);
        errorPrint("Missing command name", FPRINTF);
      }
      else if (syncheck == SYN_FAIL_MULTREDOUT){
        freeDynArray(oTokens);
        errorPrint("Multiple redirection of standard out", FPRINTF);
      }
      else if (syncheck == SYN_FAIL_NODESTOUT){
        freeDynArray(oTokens);
        errorPrint("Standard output redirection without file name", FPRINTF);
      }
      else if (syncheck == SYN_FAIL_MULTREDIN){
        freeDynArray(oTokens);
        errorPrint("Multiple redirection of standard input", FPRINTF);
      }
      else if (syncheck == SYN_FAIL_NODESTIN){
        freeDynArray(oTokens);
        errorPrint("Standard input redirection without file name", FPRINTF);
      }
        
      break;

    case LEX_QERROR:
      errorPrint("Unmatched quote", FPRINTF);
      freeDynArray(oTokens);
      break;

    case LEX_NOMEM:
      errorPrint("Cannot allocate memory", FPRINTF);
      freeDynArray(oTokens);
      break;

    case LEX_LONG:
      errorPrint("Command is too large", FPRINTF);
      freeDynArray(oTokens);
      break;

    default:
      errorPrint("lexLine needs to be fixed", FPRINTF);
      exit(EXIT_FAILURE);
  }
}

static void freeArray(StringArray *arr, int num){
    // traverse the nested array and free each StringArray
    for (int i = 0; i < num; i++){
        free(arr[i]);
    }
    free(arr);
}

static void freeDynArray(DynArray_T oTokens){
    int num_tokens = DynArray_getLength(oTokens);
    // free each token in the dynamic array
    for (int i = 0; i < num_tokens; i++){
        struct Token *t = DynArray_get(oTokens, i);
        freeToken(t,NULL);
    }
    DynArray_free(oTokens);
}

static int argProcess(DynArray_T oTokens, StringArray *argv_arr){
    // setup the variables
    int num_tokens = DynArray_getLength(oTokens);
    int num_pipe = countPipe(oTokens);
    int *num_args = (int*)calloc(num_pipe+1, sizeof(int));

    // traverse the dynamic array and process the tokens
    for (int i = 0, pipe_index = 0; i < num_tokens ; i++){
        struct Token *t = DynArray_get(oTokens, i);
        // if the token is a pipe, increment the pipe_index
        if (t->eType == TOKEN_PIPE){
            pipe_index++;
        }
        // if the token is a word, add it to the argv_arr
        else if (t->eType == TOKEN_WORD){
            argv_arr[pipe_index][num_args[pipe_index]] = t->pcValue;
            num_args[pipe_index]++;
        }
        // if the token is a redirection in, dup2 the file descriptor to stdin
        else if (t->eType == TOKEN_REDIN && pipe_index == 0){
            FILE* fp = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "r");
            if (fp == NULL){
                freeArray(argv_arr, num_pipe + 1);
                freeDynArray(oTokens);
                free(num_args);
                errorPrint("No such file or directory", FPRINTF);
                return EXIT_FAILURE;
            }
            dup2(fileno(fp), 0);
            fclose(fp);
            i+=2;
        }
        // if the token is a redirection out, dup2 the file descriptor to stdout
        else if (t->eType == TOKEN_REDOUT && pipe_index == num_pipe){
            FILE* fp = fopen((t = DynArray_get(oTokens, i+1))->pcValue, "w");
            if (fp == NULL){
                freeArray(argv_arr, num_pipe + 1);
                freeDynArray(oTokens);
                free(num_args);
                errorPrint("Cannot open file", FPRINTF);
                return EXIT_FAILURE;
            }
            if (fchmod(fileno(fp), 0600) == -1){
                freeArray(argv_arr, num_pipe + 1);
                freeDynArray(oTokens);
                free(num_args);
                errorPrint("Cannot change file mode", FPRINTF);
                return EXIT_FAILURE;
            }
            dup2(fileno(fp), 1);
            fclose(fp);
            i+=2;
        }
    }
    // for debugging
    dumpArgv(argv_arr, num_pipe + 1, num_args);

    // free the memory
    free(num_args);
    return EXIT_SUCCESS;
}

static void exec(DynArray_T oTokens) {
    enum BuiltinType btype;

    // setup the variables
    int num_pipe = countPipe(oTokens);
    int num_tokens = DynArray_getLength(oTokens);

    StringArray *argv_arr; 

    // save the stdin and stdout
    int old_stdin = dup(0);
    int old_stdout = dup(1);

    // allocate memory for commands
    argv_arr = (StringArray *)calloc(num_pipe + 1, sizeof(StringArray));

    if (argv_arr == NULL){
        errorPrint("Cannot allocate memory", FPRINTF);
        freeDynArray(oTokens);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_pipe + 1 ; i++){
        argv_arr[i] = (StringArray)calloc(num_tokens+1, sizeof(char*));
        if (argv_arr[i] == NULL){
            freeArray(argv_arr, i);
            freeDynArray(oTokens);
            errorPrint("Cannot allocate memory", FPRINTF);
            exit(EXIT_FAILURE);
        }
    }


    // check if it is a builtin command
    btype = checkBuiltin(DynArray_get(oTokens, 0));

    // if it is a builtin command
    if (btype == NORMAL){
        // set up the arguments
        // if the process fails, return
        if(argProcess(oTokens, argv_arr) == EXIT_FAILURE){
            return;
        }
        // if we need to pipelining
        if (num_pipe > 0){
            int fd[2], fdsave;

            // for the number of arguments
            for (int i = 0; i < num_pipe + 1; i++){
                // if it is not the last command
                if (i < num_pipe){
                    // make a pipe
                    if (pipe(fd) == -1){
                        freeArray(argv_arr, num_pipe + 1);
                        freeDynArray(oTokens);
                        errorPrint("Cannot create pipe", FPRINTF);
                        exit(EXIT_FAILURE);
                    }
                }
                pid_t pid = fork();
                if (pid < 0)
                {
                    freeArray(argv_arr, num_pipe + 1);
                    freeDynArray(oTokens);
                    errorPrint("Cannot fork", FPRINTF);
                    exit(EXIT_FAILURE);
                }
                // child process
                else if (pid == 0){
                    void (*pfRet) (int);
                    pfRet = signal(SIGINT, SIG_DFL);
                    assert(pfRet != SIG_ERR);
                    pfRet =signal(SIGQUIT, SIG_DFL);
                    assert(pfRet != SIG_ERR);
                    pfRet =signal(SIGALRM, SIG_DFL);
                    assert(pfRet != SIG_ERR);

                    // first command
                    if (i == 0){
                        close(fd[READ]);

                        // we need to redirect stdout to fd[1]
                        dup2(fd[WRITE], 1);
                        
                        // we need to close unused fd
                        close(fd[WRITE]);
                    }

                    // last command
                    else if (i == num_pipe){
                        // we need to redirect stdin to fdsave
                        dup2(fdsave, 0);
                        
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
                        
                        // we need to redirect stdout to fd[1]
                        dup2(fd[WRITE], 1);
                           
                        // we need to close unused fd
                        close(fd[WRITE]);

                    }
                    // execute command
                    execvp(argv_arr[i][0], argv_arr[i]);

                    // if execvp fails
                    errorPrint(argv_arr[i][0], PERROR);
                    freeArray(argv_arr, num_pipe + 1);
                    freeDynArray(oTokens);
                    exit(EXIT_FAILURE);
                }
                // parent process
                else{
                    // if it is not the last command
                    if(i < num_pipe){
                        close(fd[WRITE]);
                        fdsave = dup(fd[READ]);

                        // we need to close unused fd
                        close(fd[READ]);
                    }
                    // wait for the child process to finish
                    wait(NULL);
                }
            }
        }
        // if we don't need to pipelining
        else{
            pid_t pid = fork();
            if (pid < 0){
                errorPrint("Cannot fork", FPRINTF);
                freeArray(argv_arr, 1);
                freeDynArray(oTokens);
                exit(EXIT_FAILURE);
            }
            // child process
            else if (pid == 0){
                void (*pfRet) (int);
                pfRet = signal(SIGINT, SIG_DFL);
                assert(pfRet != SIG_ERR);
                pfRet = signal(SIGQUIT, SIG_DFL);
                assert(pfRet != SIG_ERR);
                pfRet = signal(SIGALRM, SIG_DFL);
                assert(pfRet != SIG_ERR);

                // execute command
                execvp(argv_arr[0][0], argv_arr[0]);

                // if execvp fails
                errorPrint(argv_arr[0][0], PERROR);
                freeArray(argv_arr, 1);
                freeDynArray(oTokens);
                exit(EXIT_FAILURE);
            }
            // parent process
            else{
                // wait for child process to finish
                wait(NULL);
            }
        }
        // restore stdin and stdout
        dup2(old_stdin, 0);
        dup2(old_stdout, 1);
        
        // close old fd
        close(old_stdin);
        close(old_stdout);

        // free memory
        freeArray(argv_arr, num_pipe + 1);
        freeDynArray(oTokens);

        return;
    }
    else{
        // it is a builtin command
        // we need to execute the builtin command
        freeArray(argv_arr, num_pipe + 1);
        execBuiltIn(oTokens, btype);

        return;
    }

}

static void execBuiltIn(DynArray_T oTokens, enum BuiltinType btype){
    int num_tokens = DynArray_getLength(oTokens);
    struct Token *t;
    // if it is exit
    if (btype == B_EXIT){
        // exit the shell
        if (num_tokens == 1){
            printf("\n");
            freeDynArray(oTokens);
            exit(EXIT_SUCCESS);
        }
        else{
            freeDynArray(oTokens);
            errorPrint("exit does not take any parameters", FPRINTF);\
            return;
        }
    }
    // if it is setenv
    else if (btype == B_SETENV){
        // set environment variable
        if (num_tokens == 2){
            t = DynArray_get(oTokens, 1);
            if(t->eType != TOKEN_WORD){
                freeDynArray(oTokens);
                errorPrint("setenv takes one or two parameters", FPRINTF);
                return;
            }

            char *name = t->pcValue;

            if(setenv(name, "\0", 1) == -1){
                freeDynArray(oTokens);
                errorPrint("Cannot set environment variable", FPRINTF);
                return;
            }
        }
        else if (num_tokens == 3){
            t = DynArray_get(oTokens, 1);
            if(t->eType != TOKEN_WORD){
                freeDynArray(oTokens);
                errorPrint("setenv takes one or two parameters", FPRINTF);
                return;
            }

            char *name = t->pcValue;

            t = DynArray_get(oTokens, 2);
            if(t->eType != TOKEN_WORD){
                freeDynArray(oTokens);
                errorPrint("setenv takes one or two parameters", FPRINTF);
                return;
            }

            char *value = t->pcValue;

            if(setenv(name, value, 1) == -1){
                freeDynArray(oTokens);
                errorPrint("Cannot set environment variable", FPRINTF);
                return;
            }
        }
        else{
            freeDynArray(oTokens);
            errorPrint("setenv takes one or two parameters", FPRINTF);
            return;
        }
    }
    // if it is unsetenv
    else if (btype == B_USETENV){
        // unset environment variable
        if (num_tokens == 2){
            t = DynArray_get(oTokens, 1);
            if(t->eType != TOKEN_WORD){
                freeDynArray(oTokens);
                errorPrint("unsetenv takes one parameter", FPRINTF);
                return;
            }

            char *name = t->pcValue;

            if(unsetenv(name) == -1){
                freeDynArray(oTokens);
                errorPrint("Cannot unset environment variable", FPRINTF);
                return;
            }
        }
        else{
            freeDynArray(oTokens);
            errorPrint("unsetenv takes one parameter", FPRINTF);
            return;
        }
    }
    // if it is cd
    else if (btype == B_CD){
        // change directory
        if (num_tokens == 1){
            // change to home directory
            if (chdir(getenv("HOME")) == -1){
            freeDynArray(oTokens);
            errorPrint("Cannot change directory", FPRINTF);
            return;
            }
        }
        else if (num_tokens == 2){
            // change to the specified directory
            t = DynArray_get(oTokens, 1);
            if(t->eType != TOKEN_WORD){
                freeDynArray(oTokens);
                errorPrint("cd takes one parameter", FPRINTF);
                return;
            }

            char *path = t->pcValue;

            if (chdir(path) == -1){
            freeDynArray(oTokens);
            errorPrint("No such file or directory", FPRINTF);
            return;
            }
        }
        else{
            freeDynArray(oTokens);
            errorPrint("cd takes one parameter", FPRINTF);
            return;
        }

    }
    // free tokens
    freeDynArray(oTokens);
    return;
}

static void sigHandler(int iSig){
    static int called = 0;

    // if it is SIGQUIT and it is the first time
    if(iSig == SIGQUIT && called == 0){
        fprintf(stdout, "\nType Ctrl-\\ again within 5 seconds to exit.\n");
        fflush(stdout);
        called = 1;
        alarm(5);
    }
    // if it is SIGQUIT and it is the second time
    else if(iSig == SIGQUIT && called == 1){
        exit(EXIT_SUCCESS);
    }
    // if it is SIGALRM
    else if(iSig == SIGALRM && called == 1){
        called = 0;
    }
}
