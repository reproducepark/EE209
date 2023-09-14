# --------------------------------------------------------------------
# dc.s
#
# Desk Calculator (dc) (x86-64)
# 
# Student ID: 20210251
# --------------------------------------------------------------------

    .equ   BUFFERSIZE, 32
    .equ   EOF, -1
    .equ   PADDINGSIZE, 8
    .equ   INITVALUE, 1
    .equ   OVERFLOWVALUE, -1
    .equ   MAXVALUE, 2147483647
    .equ   STACKUNITSIZE, 16
    .equ   END_SUCCESS, 0
    .equ   END_FAILURE, 1

# --------------------------------------------------------------------

.section ".rodata"

scanfFormat:
     .asciz "%s"

printfFormat:
     .asciz "%d\n"
    
stackEmptyError:
     .asciz "dc: stack empty\n"
    
divideByZeroError:
     .asciz "dc: divide by zero\n"

remainderByZeroError:
     .asciz "dc: remainder by zero\n"

overflowError:
    .asciz "dc: overflow happens\n"

# --------------------------------------------------------------------

    .section ".data"

# --------------------------------------------------------------------

    .section ".bss"

# --------------------------------------------------------------------
    .section ".text"

# -------------------------------------------------------------
# int powerfunc(int base, int exponent)
# Runs the power function.  Returns result.
# -------------------------------------------------------------

.globl	powerfunc
.type   powerfunc, @function

# --------------------------------------------------------------------
# input : base, exponent
# output: result of base^exponent

# int i = 1;
# while(base > 0){
#     i = i * base;
#     if(overflow){
#         return -1;
#     }
#     base = base - 1;
# }
# return i;
# --------------------------------------------------------------------

powerfunc:
    # prologue : save callee-saved registers
    pushq   %rbp
    subq    $PADDINGSIZE, %rsp
    movq    %rsp, %rbp
    
    # initialize i = 1
    movl    $INITVALUE, %eax

    .powerfuncloop:
        # check if base > 0
        cmpl    $0, %esi

        # if not, return i
        jle     .powerfuncloopend

        # if so, i = i * base
        imull   %edi, %eax

        # check if overflow
        jo      .powerfuncoverflow

        # base = base - 1
        decq    %rsi

        jmp     .powerfuncloop

    .powerfuncloopend:
        # epilogue : restore callee-saved registers
        addq    $PADDINGSIZE, %rsp
        popq    %rbp

        # return result (base^exponent)
        ret            

    .powerfuncoverflow:
        # epilogue : restore callee-saved registers
        addq   $PADDINGSIZE, %rsp
        popq   %rbp

        # return -1
        movq   $OVERFLOWVALUE, %rax
        ret

# -------------------------------------------------------------
# int main(void)
# Runs desk calculator program.  Returns 0.
# -------------------------------------------------------------

    .text
    .globl  main
    .type   main, @function


main:
    # prologue : save callee-saved registers
    pushq   %rbp
    movq    %rsp, %rbp

    # char buffer[BUFFERSIZE]
    # decrease stack pointer by BUFFERSIZE
    subq    $BUFFERSIZE, %rsp


# -------------------------------------------------------------
    #  <input>

    # while (1) {
    #     if(scanf("%s", buffer) == EOF){
    #         goto quit;
    #     }

    #     if (!isdigit(buffer[0])) {
    #         goto input2;
    #     }

    #     i = atoi(buffer);
    #     stack.push(i);
    # }
# -------------------------------------------------------------
.input:
    
    # load scanf format string
    leaq    scanfFormat, %rdi

    # load buffer address (rbp - BUFFERSIZE)
    leaq    -BUFFERSIZE(%rbp), %rsi
    movb    $0, %al

    # call scanf
    call    scanf

    # check if user input == EOF
    cmp	    $EOF, %eax
    je	    .quit

    # check if buffer[0] is a digit
    # move buffer[0] to %rdi (input to isdigit)
    movzbq  -BUFFERSIZE(%rbp), %rdi

    # check if buffer[0] is a digit by isdigit
    call   isdigit

    # set ZF to 1 if rax is 0
    test   %rax, %rax 

    # if buffer[0] is not a digit, jump to .input2
    je    .input2

    # if buffer[0] is a digit, translate to integer by atoi
    # move address of buffer to %rdi (input to atoi)
    leaq    -BUFFERSIZE(%rbp), %rdi
    call    atoi

    # push integer to stack
    pushq  %rax 
    subq   $PADDINGSIZE, %rsp

    jmp .input

# -------------------------------------------------------------
    #  <input2>
    # switch (buffer[0]){
    #     case '_':
    #         goto negative
    #     case 'p':
    #         goto print
    #     case 'q':
    #         goto quit
    #     case '+':
    #         goto add
    #     case '-':
    #         goto sub
    #     case '*':
    #         goto mul
    #     case '/':
    #         goto div
    #     case '%':
    #         goto mod
    #     case '^':
    #         goto pow
    #     case 'f':
    #         goto printall
    #     case 'c':
    #         goto clear
    #     default:
    #         goto input (starting point of while loop)
    # }
# -------------------------------------------------------------
.input2:

    # check if buffer[0] is p
    movb    -BUFFERSIZE(%rbp), %bl

    # check if buffer[0] is _
    cmpb     $'_', %bl
    je      .negative

    # check if buffer[0] is p
    cmpb     $'p', %bl
    je      .print

    # check if buffer[0] is q
    cmpb     $'q', %bl
    je      .quit

    # check if buffer[0] is +
    cmpb     $'+', %bl
    je      .add

    # check if buffer[0] is -
    cmpb     $'-', %bl
    je      .sub

    # check if buffer[0] is *
    cmpb     $'*', %bl
    je      .mul

    # check if buffer[0] is /
    cmpb     $'/', %bl
    je      .div

    # check if buffer[0] is %
    cmpb     $'%', %bl
    je      .mod

    # check if buffer[0] is ^
    cmpb     $'^', %bl
    je      .pow

    # check if buffer[0] is f
    cmpb     $'f', %bl
    je      .printall

    # check if buffer[0] is c
    cmpb     $'c', %bl
    je      .clear

    # if buffer[0] is not a valid command, goto .input
    jmp     .input

# -------------------------------------------------------------
    # <negative>
    # stack.push(atoi(buffer + 1) * -1);
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.negative:

    # move address of buffer to %rdi (input to atoi)
    leaq    -BUFFERSIZE+1(%rbp), %rdi
    call    atoi

    # negate the value
    negq    %rax

    # store the value of buffer into stack
    pushq  %rax
    subq   $PADDINGSIZE, %rsp

    jmp .input

# -------------------------------------------------------------
    # <print>

    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op = stack.pop()
    # printf("%d\n", op);
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.print:
    # print top of stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load top of stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # load printf format string
    leaq    printfFormat, %rdi

    # load top of stack into %rsi
    movq    %rax, %rsi

    # push back to stack
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    # call printf
    call    printf

    jmp     .input

# -------------------------------------------------------------
    # <quit>

    # exit(0);
# -------------------------------------------------------------
.quit:	

    # epilogue : restore callee-saved registers
    movq    %rbp, %rsp
    popq    %rbp

    # return 0
    movq    $END_SUCCESS, %rax
    ret

# -------------------------------------------------------------
    # <add>

    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto error2;
    # }
    # op1 = stack.pop()
    # result = op1+op2
    # if (overflow) {
    #     goto overflowerror;
    # }
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------

.add:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # add %rax and %rbx
    addl    %ebx, %eax

    # check for overflow
    jo     .overflowerror

    # push result to stack
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    jmp     .input
    
# -------------------------------------------------------------
    # <sub>
    
    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto error2;
    # }
    # op1 = stack.pop()
    # result = op1-op2
    # if (overflow) {
    #     goto overflowerror;
    # }
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.sub:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # subtract %rax and %rbx
    subl    %ebx, %eax

    # check for overflow
    jo     .overflowerror

    # push result to stack
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    jmp     .input

# -------------------------------------------------------------
    # <mul>
    
    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto error2;
    # }
    # op1 = stack.pop()
    # result = op1*op2
    # if (overflow) {
    #     goto overflowerror;
    # }
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.mul:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # multiply %rax and %rbx
    imull   %ebx, %eax

    # check for overflow
    jo     .overflowerror

    # push result to stack
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    jmp     .input

# -------------------------------------------------------------
    # <div>
    
    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto error2;
    # }
    # op1 = stack.pop()
    # if (op2 == 0) {
    #     goto zeroerror1;
    # }
    # result = op1/op2
    # if (overflow) {
    #     goto overflowerror;
    # }
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.div:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # check if %rbx is 0
    cmpq    $0, %rbx

    # if %rbx is 0, print error message
    je      .zeroerror1

    # divide %rax and %rbx
    cqto
    idivq   %rbx

    # check for overflow
    movq    $MAXVALUE, %r13
    cmpq    %r13, %rax
    jg     .overflowerror

    # push result to stackS
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    jmp     .input

# -------------------------------------------------------------
    # <mod>
    
    # if (len(stack) < 1) {
    #     goto .error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto .error2;
    # }
    # op1 = stack.pop()
    # if (op2 == 0) {
    #     goto .zerroerror2;
    # }
    # result = op1%op2
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.mod:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rax

    # check if %rbx is 0
    cmpq    $0, %rbx

    # if %rbx is 0, print error message
    je      .zeroerror2

    # divide %rax and %rbx
    cqto
    idivq   %rbx

    # store remainder in stack
    pushq   %rdx
    subq   $PADDINGSIZE, %rsp

    jmp     .input

# -------------------------------------------------------------
    # <pow>
    
    # if (len(stack) < 1) {
    #     goto error1;
    # }
    # op2 = stack.pop()
    # if (len(stack) < 1) {
    #     goto error2;
    # }
    # op1 = stack.pop()
    # result = powerfunc(op1, op2)
    # if (result == -1) {
    # goto overflowerror;
    # }
    # stack.push(result)
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.pow:
    # pop two values from stack
    # compare stack pointer -32 to base pointer
    # if stack pointer >= base pointer-32, error
    leaq    -BUFFERSIZE(%rbp), %r12
    cmpq    %r12, %rsp
    jge     .error1

    # load from stack into %rbx
    addq    $PADDINGSIZE, %rsp
    popq    %rbx
    movq    %rbx, %rsi

    # compare stack pointer to base pointer - 32
    cmpq    %r12, %rsp
    jge     .error2

    # load from stack into %rax
    addq    $PADDINGSIZE, %rsp
    popq    %rdi

    # call powerfunction
    call    powerfunc

    # if %rax is less than 0, print error message
    cmpq    $0, %rax
    jl      .overflowerror

    # push %rax to stack
    pushq   %rax
    subq   $PADDINGSIZE, %rsp

    jmp     .input

# -------------------------------------------------------------
    # <printall>
    
    # for (i = len(stack)-1; i >= 0; i--){
    #     printf("%d\n", stack[i]);
    # }
    # goto input; (starting point of while loop)
# -------------------------------------------------------------

.printall:
    # print all values in stack
    # save stack pointer
    movq    %rsp, %rbx
    leaq   -BUFFERSIZE(%rbp), %r12
    # print all values in stack
    .printloop:
        # compare stack pointer to base pointer - 32
        cmpq    %r12, %rbx
        jge     .printloopend

        # load top of stack into %rsi
        addq   $PADDINGSIZE, %rbx
        movq   (%rbx), %rsi
        subq   $PADDINGSIZE, %rbx

        # load printf format string into %rdi
        leaq    printfFormat, %rdi

        # call printf
        call    printf

        # add 16 to stack pointer
        addq    $STACKUNITSIZE, %rbx
        jmp     .printloop

    .printloopend:
        jmp     .input

# -------------------------------------------------------------
    # <clear>
    
    # stack.clear();
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.clear:
    # clear stack
    leaq  -BUFFERSIZE(%rbp), %rsp
    jmp     .input

# -------------------------------------------------------------
    # <error1>
    
    # fprintf(stderr, "dc: stack empty\n");
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.error1:
    # print error message to stderr

    # load stderr
    movq    stderr, %rdi

    # load error message
    leaq    stackEmptyError, %rsi

    # call fprintf
    call    fprintf
    jmp     .input

# -------------------------------------------------------------
    # <error1>
    
    # stack.push(op2)
    # fprintf(stderr, "dc: stack empty\n");
    # goto input; (starting point of while loop)
# -------------------------------------------------------------
.error2:
    # print error message to stderr

    # push value back to stack
    pushq   %rbx
    subq    $PADDINGSIZE, %rsp

    # load stderr
    movq    stderr, %rdi

    # load error message
    leaq    stackEmptyError, %rsi
    
    # call fprintf
    call    fprintf
    jmp     .input

# -------------------------------------------------------------
    # <zeroerror1>
    
    # fprintf(stderr, "dc: divide by zero\n");
    # exit(1);
# -------------------------------------------------------------
.zeroerror1:
    # print error message to stderr

    # load stderr
    movq    stderr, %rdi

    # load error message
    leaq    divideByZeroError, %rsi

    # call fprintf
    call    fprintf

    # epilogue : restoe callee-saved registers
    movq    %rbp, %rsp
    popq    %rbp
    
    # return 1
    movq    $END_FAILURE, %rax
    ret
# -------------------------------------------------------------
    # <zeroerror2>
    
    # stack.push(op1)
    # fprintf(stderr, "dc: remainder by zero\n");
    # exit(1);
# -------------------------------------------------------------
.zeroerror2:
    # print error message to stderr

    # load stderr
    movq    stderr, %rdi

    # load error message
    leaq    remainderByZeroError, %rsi
    
    # call fprintf
    call    fprintf
    
    # epilogue : restoe callee-saved registers
    movq    %rbp, %rsp
    popq    %rbp
    
    # return 1
    movq    $END_FAILURE, %rax
    ret

# -------------------------------------------------------------
    # <overflowerror>
    
    # fprintf(stderr, "dc: overflow happens\n");
    # exit(1);
# -------------------------------------------------------------
.overflowerror:
    # print error message to stderr

    # load stderr
    movq    stderr, %rdi
    
    # load error message
    leaq    overflowError, %rsi
    
    # call fprintf
    call    fprintf
    
    # epilogue : restoe callee-saved registers
    movq    %rbp, %rsp
    popq    %rbp

    # return 1
    movq    $END_FAILURE, %rax
    ret
