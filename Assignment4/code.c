#include <stdio.h>

int power(int base, int n) {
    int i, p;
    p = 1;

    for (i = n; i > 0; --i)
        p = p * base;
    return p;
}


while (1) {
    scanf("%s", buffer);
    if (!isdigit(buffer[0])) {
        if (buffer[0] == 'p') {
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
            } else {
                printf("%d\n", (int)stack.peek());
            }
        } 
        else if (buffer[0] == 'q') {
            return 0;
        } 
        else if (buffer[0] == '+') {
            int a, b;
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                continue;
            }
            a = (int)stack.pop();
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                stack.push(a);
                continue;
            }
            b = (int)stack.pop();
            res = a + b;
            stack.push(res);
        } 
        else if (buffer[0] == '/') {
            int a, b;
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                continue;
            }
            a = (int)stack.pop();
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                stack.push(a);
                continue;
            }
            b = (int)stack.pop();
            res = a / b;
            stack.push(res);
        } 
        else if (buffer[0] == '%') {
            int a, b;
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                continue;
            }
            a = (int)stack.pop();
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                stack.push(a);
                continue;
            }
            b = (int)stack.pop();
            res = a % b;
            stack.push(res);
        } 
        else if (buffer[0] == '^') {
            int a, b;
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                continue;
            }
            a = (int)stack.pop();
            if (stack.peek() == NULL) {
                printf("Stack is empty!\n");
                stack.push(a);
                continue;
            }
            b = (int)stack.pop();
            res = pow(a, b);
            stack.push(res);
        } 
        else if (buffer[0] == 'f'){
            for (i = len(stack)-1; i >= 0; i--) {
                printf("%d\n", stack[i]);
            }
        }
        else if (buffer[0] == 'c'){
            stack.clear();
        }
    }
    else { /* the first no. is a digit */
        int no = atoi(buffer);
        stack.push(no);
    }
}