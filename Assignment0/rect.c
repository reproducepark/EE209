#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    int a=0, b=0;
    if (argc == 1){
        scanf("%d %d", &a, &b);
    }
    else if (argc == 3){
        a = atoi(argv[1]);
        b = atoi(argv[2]);
    }
    printf("+");
    for (int j = 0; j < b; j++){
        printf("-");
    }
    printf("+\n");

    for (int i = 0; i < a; i++){
        printf("|");
        for (int j = 0; j < b; j++){
            printf(" ");
        }
        printf("|\n");
        
    }

    printf("+");
    for (int j = 0; j < b; j++){
        printf("-");
    }
    printf("+\n");
    return 0;
}
