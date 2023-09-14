#include <stdio.h>
int pow(int a, int b) {
    register int res = 1;
    for (register int i = 0; i < b; i++) {
        res *= a;
    }
    return res;
}

int main() {
    int c;
    register int a = 3;
    register int b = 10;
    int res;
    scanf("%c", &c);
    switch (c){
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            res = a / b;
            break;
        case '^':
            res = pow(a, b);
            break;
    }

    goto .end;
    .end:
    printf("%d", res);




    // if (c == '+'){
    //     res = a + b;
    // }
    // if (c == '/'){
    //     res = a / b;
    // }
    // else if (c == '%'){
    //     res = a % b;
    // }
    // else if (c == '*'){
    //     res = a * b;
    // }
    // else if (c == '-'){
    //     res = a - b;
    // }
    
    printf("%d", res);
}