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

/*
1.read characters from the standard input stream
2.count the number of newlines, words, and characters.
3.print the number of newlines, words, and characters 
  to the standard output stream
4.or print the error message to the standard error stream.
*/

int main(void){

    enum state s = NEWLINE;   //set the initial state to NEWLINE

    int num_newline = 0;      //number of newlines
    int num_word = 0;         //number of words
    int num_char = 0;         //number of characters
    int num_line_cmt_init = 0;//number of line where comment is started

    char buf;                 //buffer for reading character

    //get one character from the standard input stream
    while((buf = getchar())){

        // debug statement
        // printf("char : %c mode : %d new : %d word : %d
        //  char : %d\n", buf, s, num_newline, num_word, num_char);
         
        switch(s) //Switch according to state
        {
            /*
            state : NEWLINE (newline is started)
            value : 0

            Depending on what's in the buffer, 
            there are 5 cases in this state

            1. EOF
            It means file ends successfully. 
            Using goto to exit loop and switch.

            Except EOF, Increase num_char by 1.

            2. newline
            It means input is newline. Increase num_newline by 1.
            And change state to NEWLINE. (stay in the same state)

            3. space except newline
            It means input is space except newline.
            Change state to SPACE.

            4. slash
            It means input is first slash. Change state to SLASH_1.

            5. character expect slash 
            It means input is character except slash.
            Increase num_word by 1. Change state to CHARACTER.
            */

            case NEWLINE: 
                if (buf == EOF){
                    goto exit_s;
                }
                num_char++;
                if(isspace(buf)){
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    num_word++;
                    if(buf == '/'){
                        s = SLASH_1W;   
                    }
                    else{
                        s = CHARACTER;
                    }
                }

            break;

            /*
            state : CHARACTER (previous input is character)
            value : 1

            Depending on what's in the buffer, 
            there are 5 cases in this state

            1. EOF
            It means file ends successfully.
            Using goto to exit loop and switch.

            Except EOF, Increase num_char by 1.

            2. newline
            It means input is newline. Increase num_newline by 1.
            And change state to NEWLINE.

            3. space except newline
            It means input is space except newline.
            Change state to SPACE.

            4. slash
            It means input is first slash. Change state to SLASH_1.

            5. character expect slash 
            It means input is character except slash.
            Change state to CHARACTER. (stay in the same state)
            */

            case CHARACTER: 
                if (buf == EOF){
                    goto exit_s;
                }
                num_char++;
                if(isspace(buf)){
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    if(buf == '/'){
                        s = SLASH_1;
                    }
                    else{
                        s = CHARACTER;
                    }
                }

            break;

            /*
            state : SPACE (previous input is space)
            value : 2

            Depending on what's in the buffer,
            there are 5 cases in this state

            Except EOF, Increase num_char by 1.

            1. EOF
            It means file ends successfully.
            Using goto to exit loop and switch.

            2. newline
            It means input is newline. Increase num_newline by 1.
            And change state to NEWLINE.

            3. space except newline
            It means input is space except newline.
            Change state to SPACE. (stay in the same state)

            4. slash
            It means input is first slash. Change state to SLASH_1.

            5. character expect slash
            It means input is character except slash.
            Increase num_word by 1. Change state to CHARACTER.
            */

            case SPACE: 
                if (buf == EOF){
                    goto exit_s;
                }
                num_char++;
                if(isspace(buf)){
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    if(buf == '/'){
                        num_word++;
                        s = SLASH_1W;   
                    }
                    else{
                        num_word++;
                        s = CHARACTER;
                    }
                }

            break;

            /*
            state : SLASH_1 (previous input is slash)
            value : 3

            Depending on what's in the buffer,
            there are 6 cases in this state. 

            Except EOF, star, slash, Increase num_char by 1.

            1. EOF
            It means file ends successfully.
            Using goto to exit loop and switch.

            2. newline
            It means input is newline. Increase num_newline by 1.
            And change state to NEWLINE.

            3. space except newline
            It means input is space except newline.
            Change state to SPACE.

            For the star and slash, we decrease num_char by 1.
            Because we don't count the previous slash as a character.
            And, we don't count the star or slash as a character.

            4. star
            It means input is first star. Change state to STAR_1.
            And set num_line_cmt_init to num_newline.
            We check the line where comment is started.

            5. slash
            It means input is second slash. Change state to SLASH_2.

            6. character expect star and slash
            It means input is character except star and slash.
            Change state to CHARACTER.
            */

            case SLASH_1: 
                if (buf == EOF){
                    goto exit_s;
                }
                else if(isspace(buf)){
                    num_char++;
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    if(buf == '*'){
                        num_line_cmt_init = num_newline+1;
                        num_char--;
                        s = STAR_1;
                    }
                    else if(buf == '/'){
                        num_char--;
                        s = SLASH_2;
                    }
                    else{
                        num_char++;
                        s = CHARACTER;
                    }
                }
            
            break;

            /*
            수정필요
            state : SLASH_1W    (previous input is slash)
            value : 3

            Depending on what's in the buffer,
            there are 6 cases in this state. 

            Except EOF, star, slash, Increase num_char by 1.

            1. EOF
            It means file ends successfully.
            Using goto to exit loop and switch.

            2. newline
            It means input is newline. Increase num_newline by 1.
            And change state to NEWLINE.

            3. space except newline
            It means input is space except newline.
            Change state to SPACE.

            For the star and slash, we decrease num_char by 1.
            Because we don't count the previous slash as a character.
            And, we don't count the star or slash as a character.

            4. star
            It means input is first star. Change state to STAR_1.
            And set num_line_cmt_init to num_newline.
            We check the line where comment is started.

            5. slash
            It means input is second slash. Change state to SLASH_2.

            6. character expect star and slash
            It means input is character except star and slash.
            Change state to CHARACTER.
            */

            case SLASH_1W:   
                if (buf == EOF){
                    goto exit_s;
                }
                else if(isspace(buf)){
                    num_char++;
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    if(buf == '*'){
                        num_line_cmt_init = num_newline+1;
                        num_char--;
                        num_word--;
                        s = STAR_1;
                    }
                    else if(buf == '/'){
                        num_char--;
                        num_word--;
                        s = SLASH_2;
                    }
                    else{
                        num_char++;
                        s = CHARACTER;
                    }
                }
            
            break;
            /*
            state : SLASH_2 (previous input is second slash)
            value : 4
            Now we are in the line comment.

            Depending on what's in the buffer,
            there are 3 cases in this state.

            1. EOF
            It means file ends successfully.
            Using goto to exit loop and switch.

            2. newline
            It means input is newline, so line comment ends.
            Increase num_newline and num_char by 1. 
            And change state to NEWLINE.

            3. other characters
            It means input is other characters, 
            so line comment continues.
            */

            case SLASH_2: 
                if (buf == EOF){
                    goto exit_s;
                }
                else if(buf == '\n'){
                    num_newline++;
                    num_char++;
                    s = NEWLINE;
                }
                else{
                    s = SLASH_2;
                }

            break;

            /*
            state : STAR_1 (previous input is star)
            value : 5
            Now we are in the block comment.

            Depending on what's in the buffer,
            there are 4 cases in this state.

            1. EOF
            It means file ends unsuccessfully. 
            Using goto to exit loop and switch.

            2. newline
            It means input is newline, so block comment continues.
            Increase num_newline and num_char by 1.
            Change state to STAR_1. (stay in the same state)

            3. star
            It means input is star. Change state to STAR_2.
            In STAR_2, we can judge if it is end of block comment.

            4. other characters
            It means input is other characters,
            so block comment continues.
            */

            case STAR_1: 
                if (buf == EOF){
                    goto exit_f;
                } 
                else if(buf == '\n'){
                    num_newline++;
                    num_char++;
                    s = STAR_1;
                }
                else if(buf == '*'){
                    s = STAR_2;
                }
                else{
                    s = STAR_1;
                }

            break;

            /*
            state : STAR_2 (previous input is star)
            value : 6
            We need to judge if it is end of block comment.

            Depending on what's in the buffer,
            there are 4 cases in this state.

            1. EOF
            It means file ends unsuccessfully.
            Using goto to exit loop and switch.

            2. newline
            It means input is newline, so block comment continues.
            Increase num_newline and num_char by 1.
            Change state to STAR_1.

            3. slash
            It means input is slash, so block comment ends.
            Increase num_char by 1. 
            Cause we assume block commment as one character.
            And change state to SLASH_END.

            4. star
            It means input is star. 
            Change state to STAR_2. (stay in the same state)

            5. other characters
            It means input is other characters, 
            so block comment continues.
            Change state to STAR_1.
            */
        
            case STAR_2: 
                if (buf == EOF){
                    goto exit_f;
                }
                else if(buf == '\n'){
                    num_newline++;
                    num_char++;
                    s = STAR_1;
                }
                else if(buf == '/'){
                    num_char++;
                    s = SLASH_END;
                }
                else if(buf == '*'){
                    s = STAR_2;
                }
                else{
                    s = STAR_1;
                }

            break;

            /*
            state : SLASH_END (previous input is ending slash)
            value : 7
            
            It's behavior is identical to NEWLINE state. 
            But, this state is distinguished as a different state
            */

            case SLASH_END: 
                if (buf == EOF){
                    goto exit_s;
                }
                num_char++;
                if(isspace(buf)){
                    if(buf == '\n'){
                        num_newline++;
                        s = NEWLINE;
                    }
                    else{
                        s = SPACE;
                    }
                }
                else{
                    if(buf == '/'){
                        num_word++;
                        s = SLASH_1W;   
                    }
                    else{
                        num_word++;
                        s = CHARACTER;
                    }
                }

            break;

        }
    }
    // exit_s means file ends successfully.
    // print the result to stdout.
    exit_s: 
        printf("%d %d %d\n",num_newline, num_word, num_char);
        return EXIT_SUCCESS; //return 0
    
    // exit_f means file ends unsuccessfully.
    // print the error message to stderr.
    exit_f:
        fprintf(stderr, 
                "Error: line %d: unterminated comment\n",
                num_line_cmt_init);
        return EXIT_FAILURE; //return 1

}
