//20210251 jaehyun park, EE209 assignment 1, wc209.c

//included libraries
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


//DFA states
//detail discirption of each state is in DFA.pdf
enum state {
    NEWLINE = 0,    //state that newline is started, 
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
                    //it is similar to the NEWLINE state, value is 8

typedef struct _value{
    int num_newline;      //number of newlines
    int num_word;         //number of words
    int num_char;         //number of characters
    int num_line_cmt_init;//number of line where comment is started
    enum state s;
} Value;

/*
1.read characters from the standard input stream
2.count the number of newlines, words, and characters.
3.print the number of newlines, words, and characters 
  to the standard output stream
4.or print the error message to the standard error stream.
*/

Value newline(Value v, char buf){
    v.num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        v.num_word++;
        if(buf == '/'){
            v.s = SLASH_1W;   
        }
        else{
            v.s = CHARACTER;
        }
    }
    return v;
}

Value character(Value v, char buf){
    v.num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        if(buf == '/'){
            v.s = SLASH_1;
        }
        else{
            v.s = CHARACTER;
        }
    }
    return v;
}

Value space(Value v, char buf){
    v.num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        if(buf == '/'){
            v.num_word++;
            v.s = SLASH_1W;   
        }
        else{
            v.num_word++;
            v.s = CHARACTER;
        }
    }
    return v;
}

Value slash_1(Value v, char buf){
    if(isspace(buf)){
        v.num_char++;
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        if(buf == '*'){
            v.num_line_cmt_init = v.num_newline+1;
            v.num_char--;
            v.s = STAR_1;
        }
        else if(buf == '/'){
            v.num_char--;
            v.s = SLASH_2;
        }
        else{
            v.num_char++;
            v.s = CHARACTER;
        }
    }
    return v;
}

Value slash_1w(Value v, char buf){
    if(isspace(buf)){
        v.num_char++;
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        if(buf == '*'){
            v.num_line_cmt_init = v.num_newline+1;
            v.num_char--;
            v.num_word--;
            v.s = STAR_1;
        }
        else if(buf == '/'){
            v.num_char--;
            v.num_word--;
            v.s = SLASH_2;
        }
        else{
            v.num_char++;
            v.s = CHARACTER;
        }
    }
    return v;
}

Value slash_2(Value v, char buf){
    if(buf == '\n'){
        v.num_newline++;
        v.num_char++;
        v.s = NEWLINE;
    }
    else{
        v.s = SLASH_2;
    }
    return v;
}

Value star_1(Value v, char buf){
    if(buf == '\n'){
        v.num_newline++;
        v.num_char++;
        v.s = STAR_1;
    }
    else if(buf == '*'){
        v.s = STAR_2;
    }
    else{
        v.s = STAR_1;
    }
    return v;
}

Value star_2(Value v, char buf){
    if(buf == '\n'){
        v.num_newline++;
        v.num_char++;
        v.s = STAR_1;
    }
    else if(buf == '/'){
        v.num_char++;
        v.s = SLASH_END;
    }
    else if(buf == '*'){
        v.s = STAR_2;
    }
    else{
        v.s = STAR_1;
    }
    return v;
}

Value slash_end(Value v, char buf){
    v.num_char++;
    if(isspace(buf)){
        if(buf == '\n'){
            v.num_newline++;
            v.s = NEWLINE;
        }
        else{
            v.s = SPACE;
        }
    }
    else{
        if(buf == '/'){
            v.num_word++;
            v.s = SLASH_1W;   
        }
        else{
            v.num_word++;
            v.s = CHARACTER;
        }
    }
    return v;
}

int main(void){

    Value v;
    v.num_newline = 0;
    v.num_word = 0;
    v.num_char = 0;
    v.num_line_cmt_init = 0;
    v.s = NEWLINE;

    char buf;                 //buffer for reading character

    //get one character from the standard input stream
    while((buf = getchar())){

        // debug statement
        // printf("char : %c mode : %d new : %d word : %d
        //  char : %d\n", buf, s, num_newline, num_word, num_char);
         
        switch(v.s) //Switch according to state
        {

            case NEWLINE:
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = newline(v, buf);
                }
            break;

            case CHARACTER:
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = character(v, buf);
                }
            break;

            case SPACE: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = space(v, buf);
                }
            break;

            case SLASH_1: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = slash_1(v, buf);
                }
            
            break;

            case SLASH_1W:   
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = slash_1w(v, buf);
                }
            
            break;

            case SLASH_2: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = slash_2(v, buf);
                }

            break;

            case STAR_1: 
                if (buf == EOF){
                    goto exit_f;
                } 
                else{
                v = star_1(v, buf);
                }

            break;
        
            case STAR_2: 
                if (buf == EOF){
                    goto exit_f;
                }
                else{
                v = star_2(v, buf);
                }

            break;

            case SLASH_END: 
                if (buf == EOF){
                    goto exit_s;
                }
                else{
                v = slash_end(v, buf);
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

