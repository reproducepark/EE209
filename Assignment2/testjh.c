#include "str.h"
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define PRINT_RESULT(a) \
  (a) ? printf("Correct!\n") : printf("Wrong!\n")

int main(){
    int n = 0;
    scanf("%d", &n);
    //get the string from the stdio and convert it to long int
    for(int i = 0; i < n; i++){
        char *str = (char *)malloc(100 * sizeof(char));
        scanf("%s", str);
        long int num1 = strtol(str, NULL, 10);
        long int num2 = StrToLong(str, NULL, 10);
        printf("Your      Answer: [%ld]\n", num1);
        printf("String.h  Answer: [%ld]\n", num2);
        PRINT_RESULT(num1 == num2);
    }
}