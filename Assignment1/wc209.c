//20210251 jaehyun park, EE209 assignment 1, wc209.c

//included libraries
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

//DFA states
//detail discirption of each state is in DFA.pdf
enum state {
    NEWLINE = 0,    //state that newline is started, value is 0
                    //and it is the initial state

    CHARACTER,      //state that previous input is character, value is 1

    SPACE,          //state that previous input is space, value is 2

    SLASH_1,        //state that previous input is slash, value is 3
                    //check if it is a comment or not

    SLASH_1W,       //state that previous input is slash, value is 4
                    //check if it is a comment or not
                    //it is special case of SLASH_1, which can be word      

    SLASH_2,        //state that previous two inputs are (//) 
                    //and we are in the comment, value is 5

    STAR_1,         //state that previous two inputs are (/*)
                    //and we are in the comment, value is 6

    STAR_2,         //state that prev input is second star of comment
                    //check if it is the end of the comment, value is 7

    SLASH_END};     //state that previous input is comment ending slash
                    //it is identical to the NEWLINE state, value is 8

//struct that stores the values of each state
typedef struct _value{
    int num_newline;      //number of newlines
    int num_word;         //number of words
    int num_char;         //number of characters
    int num_line_cmt_init;//number of line where comment is started
    enum state s;
} Value;

//predeclaration of functions
//all functions below get values by reference and buffer
//and change the values.

void newline(Value *v, char buf);
void character(Value *v, char buf);
void space(Value *v, char buf);
void slash_1(Value *v, char buf);
void slash_1w(Value *v, char buf);
void slash_2(Value *v, char buf);
void star_1(Value *v, char buf);
void star_2(Value *v, char buf);
void slash_end(Value *v, char buf);

/*
1.read characters from the standard input stream
2.count the number of newlines, words, and characters.
3.if program ends successfully, print the number of newlines, 
words, and characters to the standard output stream
4.else (unterminated comment occurs) print the error message
to the standard error stream
*/

int main(void){

    //initialize the values
    Value v;
    v.num_newline = 0;        //number of newlines
    v.num_word = 0;           //number of words
    v.num_char = 0;           //number of characters
    v.num_line_cmt_init = 0;  //number of line where comment is started
    v.s = NEWLINE;            //set initial state to NEWLINE

    char buf;                 //buffer for reading character

    //get one character from the standard input stream
    while((buf = getchar())){
        
        switch(v.s) //Switch according to state
        {
            /*
            state is NEWLINE(0)
            if buf is EOF goto exit_s (exit success)
            else call newline function
            */

            case NEWLINE:
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                newline(&v, buf);
                }

            break;

            /*
            state is CHARACTER(1)
            if buf is EOF goto exit_s (exit success)
            else call character function
            */

            case CHARACTER:
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                character(&v, buf);
                }

            break;

            /*
            state is SPACE(2)
            if buf is EOF goto exit_s (exit success)
            else call space function
            */

            case SPACE: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                space(&v, buf);
                }

            break;

            /*
            state is SLASH_1(3)
            if buf is EOF goto exit_s (exit success)
            else call slash_1 function
            */

            case SLASH_1: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                slash_1(&v, buf);
                }
            
            break;

            /*
            state is SLASH_1W(4)
            if buf is EOF goto exit_s (exit success)
            else call slash_1w function
            */

            case SLASH_1W:   
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                slash_1w(&v, buf);
                }
            
            break;

            /*
            state is SLASH_2(5)
            if buf is EOF goto exit_s (exit success)
            else call slash_2 function
            */

            case SLASH_2: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                slash_2(&v, buf);
                }

            break;

            /*
            state is STAR_1(6)
            if buf is EOF goto exit_f (exit failure)
            else call star_1 function
            */

            case STAR_1: 
                if (buf == EOF){
                    goto exit_f;
                } 
                else{
                star_1(&v, buf);
                }

            break;
        
            /*
            state is STAR_2(7)
            if buf is EOF goto exit_f (exit failure)
            else call star_2 function
            */

            case STAR_2: 
                if (buf == EOF){
                    goto exit_f;
                }
                else{
                star_2(&v, buf);
                }

            break;

            /*
            state is SLASH_END(8)
            if buf is EOF goto exit_s (exit success)
            else call slash_end function
            */
            case SLASH_END: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                slash_end(&v, buf);
                }

            break;

        }
    }
    // exit_s means file ends successfully.
    // print the result to stdout.
    exit_s: 
        printf("%d %d %d\n",v.num_newline, v.num_word, v.num_char);
        return EXIT_SUCCESS; //return 0
    
    // exit_f means file ends unsuccessfully.
    // print the error message to stderr.
    exit_f:
        fprintf(stderr, 
                "Error: line %d: unterminated comment\n",
                v.num_line_cmt_init);
        return EXIT_FAILURE; //return 1

}

//definition of functions
//all functions below get values and buffer, and change the values
//functions can access the values by reference

/* 
'newline' function handles the state NEWLINE(0)

Depending on what's in the buffer, there are 4 cases in this state
and function can changes the values and state.

For all cases, we need to increase num_char by 1

1. newline
It means input is newline. Increase num_newline by 1.
And change state to NEWLINE. (stay in the same state)

2. space except newline
It means input is space except newline.
Change state to SPACE.

For the slash and character cases, we need to increase num_word by 1

3. slash
It means input is first slash. And slash can be a word.
Change state to SLASH_1W.

4. character except slash 
It means input is character except slash.
Increase num_word by 1. Change state to CHARACTER.
*/

void newline(Value *p, char buf){
    p->num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        p->num_word++;
        if(buf == '/'){
            p->s = SLASH_1W;   
        }
        else{
            p->s = CHARACTER;
        }
    }
}

/*
'character' function handles the state CHARACTER(1)

Depending on what's in the buffer, there are 4 cases in this state
and function can changes the values and state.

For all cases, we need to increase num_char by 1

1. newline
It means input is newline. Increase num_newline by 1.
And change state to NEWLINE.

2. space except newline
It means input is space except newline.
Change state to SPACE.

3. slash
It means input is first slash. Change state to SLASH_1.

4. character except slash 
It means input is character except slash.
Change state to CHARACTER. (stay in the same state)
*/

void character(Value *p, char buf){
    p->num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        if(buf == '/'){
            p->s = SLASH_1;
        }
        else{
            p->s = CHARACTER;
        }
    }
}

/*
'space' function handles the state SPACE(2)

Depending on what's in the buffer, there are 4 cases in this state
and function can changes the values and state.

For all cases, we need to increase num_char by 1

1. newline
It means input is newline. Increase num_newline by 1.
And change state to NEWLINE. (stay in the same state)

2. space except newline
It means input is space except newline.
Change state to SPACE.

For the slash and character cases, we need to increase num_word by 1

3. slash
It means input is first slash. And slash can be a word.
Change state to SLASH_1W.

4. character except slash 
It means input is character except slash.
Change state to CHARACTER.
*/

void space(Value *p, char buf){
    p->num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        p->num_word++;
        if(buf == '/'){
            p->s = SLASH_1W;   
        }
        else{
            p->s = CHARACTER;
        }
    }
}

/*

'slash_1' function handles the state SLASH_1(3)

Depending on what's in the buffer, there are 5 cases in this state
and function can changes the values and state.

Except EOF, star, slash, Increase num_char by 1.

1. newline
It means input is newline. Increase num_newline by 1.
And change state to NEWLINE.

2. space except newline
It means input is space except newline.
Change state to SPACE.

For the star and slash, we decrease num_char by 1.
Because we don't count the previous slash as a character.
And, we don't count the star or slash as a character.

3. star
It means input is first star. Change state to STAR_1.
And set num_line_cmt_init to num_newline.
We check the line where comment is started.

4. slash
It means input is second slash. Change state to SLASH_2.

5. character expect star and slash
It means input is character except star and slash.
Change state to CHARACTER.

*/

void slash_1(Value *p, char buf){
    if(isspace(buf)){
        p->num_char++;
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        if(buf == '*'){
            p->num_line_cmt_init = p->num_newline+1;
            p->num_char--;
            p->s = STAR_1;
        }
        else if(buf == '/'){
            p->num_char--;
            p->s = SLASH_2;
        }
        else{
            p->num_char++;
            p->s = CHARACTER;
        }
    }
}

/*

'slash_1w' function handles the state SLASH_1W(4)

Depending on what's in the buffer, there are 5 cases in this state
and function can changes the values and state.

It is almost identical to the slash_1 function.
But in this case we assume that slash is a word.
So, for star and slash, we decrease num_word by 1.
cause we don't count the previous slash as a word.

*/

void slash_1w(Value *p, char buf){
    if(isspace(buf)){
        p->num_char++;
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        if(buf == '*'){
            p->num_line_cmt_init = p->num_newline+1;
            p->num_char--;
            p->num_word--;
            p->s = STAR_1;
        }
        else if(buf == '/'){
            p->num_char--;
            p->num_word--;
            p->s = SLASH_2;
        }
        else{
            p->num_char++;
            p->s = CHARACTER;
        }
    }
}

/*

'slash_2' function handles the state SLASH_2(5)
Now we are in the line comment.

Depending on what's in the buffer, there are 2 cases in this state
and function can changes the values and state.

1. newline
It means input is newline, so line comment ends.
Increase num_newline and num_char by 1. 
And change state to NEWLINE.

2. other characters
It means input is other characters, so line comment continues.
Change state to SLASH_2. (stay in the same state)

*/

void slash_2(Value *p, char buf){
    if(buf == '\n'){
        p->num_newline++;
        p->num_char++;
        p->s = NEWLINE;
    }
    else{
        p->s = SLASH_2;
    }
}

/*

'star_1' function handles the state STAR_1(6)
Now we are in the block comment.

Depending on what's in the buffer, there are 3 cases in this state
and function can changes the values and state.

1. newline
It means input is newline, so block comment continues.
Increase num_newline and num_char by 1.
Change state to STAR_1. (stay in the same state)

2. star
It means input is star. Change state to STAR_2.
In STAR_2, we can judge if it is end of block comment.

3. other characters
It means input is other characters,
so block comment continues.
Change state to STAR_1. (stay in the same state)

*/

void star_1(Value *p, char buf){
    if(buf == '\n'){
        p->num_newline++;
        p->num_char++;
        p->s = STAR_1;
    }
    else if(buf == '*'){
        p->s = STAR_2;
    }
    else{
        p->s = STAR_1;
    }
}

/*

'star_2' function handles the state STAR_2(7)
We need to judge if it is end of block comment.

Depending on what's in the buffer, there are 4 cases in this state
and function can changes the values and state.

1. newline
It means input is newline, so block comment continues.
Increase num_newline and num_char by 1.
Change state to STAR_1.

2. slash
It means input is slash, so block comment ends.
Increase num_char by 1. Cause we assume block commment as one character.
And change state to SLASH_END.

3. star
It means input is star. 
Change state to STAR_2. (stay in the same state)

4. other characters
It means input is other characters, so block comment continues.
Change state to STAR_1.

*/

void star_2(Value *p, char buf){
    if(buf == '\n'){
        p->num_newline++;
        p->num_char++;
        p->s = STAR_1;
    }
    else if(buf == '/'){
        p->num_char++;
        p->s = SLASH_END;
    }
    else if(buf == '*'){
        p->s = STAR_2;
    }
    else{
        p->s = STAR_1;
    }
}

/*

'slash_end' function handles the state SLASH_END(8)
We are in the state where block comment ends.

It's behavior is identical to NEWLINE state. 
But, this state is distinguished as a different state

*/

void slash_end(Value *p, char buf){
    p->num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            p->num_newline++;
            p->s = NEWLINE;
        }
        else{
            p->s = SPACE;
        }
    }
    else{
        p->num_word++;
        if(buf == '/'){
            p->s = SLASH_1W;
        }
        else{
            p->s = CHARACTER;
        }
    }
}

