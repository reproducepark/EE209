/* Seungjae Lim / Assignment1 / decomment.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/*
header file
stdio.h for printf()
stdlib.h for EXIT_SUCCESS, EXIT_FAILURE
assert.h for assert()
*/

#define BEGIN 0
#define CHECK_COMMENT_START 1
#define COMMENT 2
#define CHECK_COMMENT_END 3
#define CHARACTER_CONSTANT 4
#define CHARACTER_BACKSLASH 5
#define STRING_CONSTANT 6
#define STRING_BACKSLASH 7
#define END_SUCCESS 8
#define END_FAILURE 9
/*
MACRO of dfa's state
BEGIN : 0 
- initial state, The most common situation
CHECK_COMMENT_START : 1 
- when meet '/', judge is it starting comment '/'+'*'
COMMENT : 2 
- in the comment, part to be deleted
CHECK_COMMENT_END : 3 
- when meet '*', judge is it ending comment '*'+'/'
CHARACTER_CONSTANT : 4 
- when meet ''', input is character constant
CHARACTER_BACKSLASH : 5 
- '\' and the following character as the ordinary characters
STRING_CONSTANT : 6 
- when meet ''', input is string
STRING_BACKSLASH : 7 
- '\' and the following character as the ordinary characters
END_SUCCESS : 8 
- final state, decomment is end success
END_FAILURE : 9 
- final state, decomment is end failure
*/

void increase_line(int* line_cnt);
void dfa_begin_handler(int buffer, 
                       int* state, 
                       int* line_cnt);
void dfa_check_comment_start_handler(int buffer, 
                                     int* state,
                                     int* line_cnt, 
                                     int* comment_start_line);
void dfa_comment_handler(int buffer, 
                         int* state, 
                         int* line_cnt);
void dfa_check_comment_end_handler(int buffer, 
                                   int* state, 
                                   int* line_cnt);
void dfa_character_constant_handler(int buffer, 
                                    int* state, 
                                    int* line_cnt);
void dfa_character_backslash_handler(int buffer, 
                                     int* state, 
                                    int* line_cnt);
void dfa_string_constant_handler(int buffer, 
                                 int* state, 
                                 int* line_cnt);
void dfa_string_backslash_handler(int buffer, 
                                  int* state, 
                                  int* line_cnt);
int is_dfa_error(int* state);
int decomment_error_handler(int comment_start_line);
int decomment_success_handler(int* state);
/*
function declaration

increase_line
increase line number, newline on stdout

dfa_begin_handler
handler of state BEGIN

dfa_check_comment_start_handler
handler of state CHECK_COMMENT_START

dfa_comment_handler
handler of state COMMENT

dfa_check_comment_end_handler
handler of state CHECK_COMMENT_END

dfa_character_constant_handler
handler of state BEGIN CHARACTER_CONSTANT

dfa_character_backslash_handler 
handler of state CHARACTER_BACKSLASH

dfa_string_constant_handler 
handler of state STRING_CONSTANT

dfa_string_backslash_handler 
handler of state STRING_BACKSLASH

is_dfa_error
determine dfa end with error(return 1), else(return 0)

decomment_error_handler 
put error message in stderr, return EXIT_FAILURE

decomment_success_handler 
return EXIT_SUCCESS
*/

/*
int main() function
input : void
return : EXIT_SUCCESS when success, EXIT_FAILURE when failure()
until end of file, 
get one character in buffer and do response based on current state
after decommenting, 
return whether there is an error
*/
int main(){
    int buffer, state = BEGIN, line_cnt = 1, comment_start_line;
    /*
    buffer : save one char temporarily, use int because of EOF
    state : state value of DFA (0~9)
    line_cnt : record line number
    comment_start_line : save line numver when comment start
    */
    while((buffer = getchar()) != EOF)
    /* buffer get char until file is end */
    {
        switch(state)
        /*control statement about DFA state*/ 
        {
            case BEGIN:
                dfa_begin_handler(buffer, 
                                  &state, 
                                  &line_cnt);
                /*
                    when state : BEGIN
                    excute dfa_begin_handler
                 */
            break;

            case CHECK_COMMENT_START:
                dfa_check_comment_start_handler(buffer, 
                                                &state, 
                                                &line_cnt, 
                                                &comment_start_line);
                /*
                    when state : CHECK_COMMENT_START
                    excute dfa_check_comment_start_handler
                 */ 
            break;

            case COMMENT:
                dfa_comment_handler(buffer, 
                                    &state, 
                                    &line_cnt);
                /*
                    when state : COMMENT
                    excute dfa_comment_handler
                 */
            break;

            case CHECK_COMMENT_END:
                dfa_check_comment_end_handler(buffer, 
                                              &state, 
                                              &line_cnt);
                /*
                    when state : CHECK_COMMENT_END
                    excute dfa_check_comment_end_handler
                 */
            break;

            case CHARACTER_CONSTANT:
                dfa_character_constant_handler(buffer, 
                                               &state, 
                                               &line_cnt);
                /*
                    when state : CHARACTER_CONSTANT
                    excute dfa_character_constant_handler
                 */
            break;

            case CHARACTER_BACKSLASH:
                dfa_character_backslash_handler(buffer, 
                                                &state, 
                                                &line_cnt);
                /*
                    when state : CHARACTER_BACKSLASH
                    excute dfa_character_backslash_handler
                 */
            break;

            case STRING_CONSTANT:
                dfa_string_constant_handler(buffer, 
                                            &state, 
                                            &line_cnt);
                /*
                    when state : STRING_CONSTANT
                    excute dfa_string_constant_handler
                 */
            break;

            case STRING_BACKSLASH:
                dfa_string_backslash_handler(buffer, 
                                             &state, 
                                             &line_cnt);
                /*
                    when state : STRING_BACKSLASH
                    excute dfa_string_backslash_handler
                 */
            break;

            default:
                assert(0);
                /*
                    when state is not define
                    make error by assert()
                */
            break;
        }
    }
    /* decommenting is finish */
    if(is_dfa_error(&state))
    /*
        is_dfa_error return
        0 : if dfa end success
        1 : if dfa end with error
    */
    {
        return decomment_error_handler(comment_start_line);
        /* error handler to print on stderr, make state END_FAILURE*/
    }
    else
    {
        return decomment_success_handler(&state);
        /* return EXIT_SUCCESS, make state END_SUCCESS*/
    }
}

/*
increase_line function
input : address of line_cnt
return : - (add one on line_cnt variable, newline on stdout)
get pointer of line_cnt in main function, add one on it
*/
void increase_line(int* line_cnt)
{
   (*line_cnt)++;
    printf("\n");
    /* add one on line_cnt variable, newline on stdout */
    return;
}

/*
dfa_begin_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
initial state, do action when buffer is '\n' '/' ''' '"'
*/
void dfa_begin_handler(int buffer, 
                       int* state, 
                       int* line_cnt)
{
    if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    else if(buffer == '/')
    {
        *state = CHECK_COMMENT_START;
        /* when buffer is '/', 
           move state to CHECK_COMMENT_START */
    }
    else if(buffer == 0x27)
    {
        *state = CHARACTER_CONSTANT;
        printf("%c",buffer);
        /* when buffer is ''', 
           move state to CHARACTER_CONSTANT, print(') */
    }
    else if(buffer == '"')
    {
        *state = STRING_CONSTANT;
        printf("%c",buffer);
        /* when buffer is '"', 
           move state to STRING_CONSTANT, print(") */
    }
    else
    {
        printf("%c",buffer);
        /* buffer isn't specific character, 
           print buffer */
    }
    return;
}

/*
dfa_check_comment_start_handler function
input : value of buffer, address of state, 
        address of line_cnt, address of comment_start_line
return : - (change state's value, 
            add line_cnt's value, save comment_start_line)
before go into comment, check is it /+* 
do action when buffer is '\n' '*' '/' ''' '"'
*/
void dfa_check_comment_start_handler(int buffer, 
                                     int* state, 
                                     int* line_cnt, 
                                     int* comment_start_line)
{
    if(buffer == '*')
    {
        printf(" ");
        *state = COMMENT;
        *comment_start_line = *line_cnt;
        /* when buffer is '*' start comment
           save line number to handling error */
    }
    else if(buffer == '/')
    {
        printf("/");
        /* when buffer is '/', 
           keep in check is it comment */
    }
    else if(buffer == '\n')
    {
        printf("/");
        increase_line(line_cnt);
        *state = BEGIN;
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout 
           move to state BEGIN*/
    }
    else if(buffer == 0x27)
    {
        *state = CHARACTER_CONSTANT;
        printf("/%c",buffer);
        /* when buffer is ''', 
           move state to CHARACTER_CONSTANT, print(') */
    }
    else if(buffer == '"')
    {
        *state = STRING_CONSTANT;
        printf("/%c",buffer);
        /* when buffer is '"', 
           move state to STRING_CONSTANT, print(") */
    }
    else
    {
        *state = BEGIN;
        printf("/%c",buffer);
        /* buffer isn't specific character,
           print / and next char and move to state BEGIN */
    }
    return;
}

/*
dfa_comment_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of comment,
don't print for decomment 
do action when buffer is '\n' '*'
*/
void dfa_comment_handler(int buffer, 
                         int* state, 
                         int* line_cnt)
{
    if(buffer == '*')
    {
        *state = CHECK_COMMENT_END;
        /* when buffer is '/', 
           move state to CHECK_COMMENT_END for check end comment */
    }
    else if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    return;
}

/*
dfa_check_comment_end_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of comment,
don't print for decomment 
do action when buffer is '\n' '/' '*'
*/
void dfa_check_comment_end_handler(int buffer, 
                                   int* state, 
                                   int* line_cnt)
{
    if(buffer == '/')
    {
        *state = BEGIN;
        /* when buffer is '/', end of comment
        move state to BEGIN */
    }
    else if(buffer == '*')
    {
        return;
        /* when buffer is '\n', 
        keep in state CHECK_COMMENT_END */
    }
    else if(buffer == '\n')
    {
        increase_line(line_cnt);
        *state = COMMENT;
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout 
           keep in comment, so move state to COMMENT */
    }
    else
    {
        *state = COMMENT;
        /* buffer isn't specific character,
           keep in comment, so move state to COMMENT */
    }
    return;
}

/*
dfa_character_constant_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of character constant,
don't make comment 
do action when buffer is '\n' '\' '''
*/
void dfa_character_constant_handler(int buffer, 
                                    int* state, 
                                    int* line_cnt)
{
    if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    else if(buffer == 0x5C)
    {
        *state = CHARACTER_BACKSLASH;
        printf("%c",buffer);
        /* when buffer is '\', 
           move state to CHARACTER_BACKSLASH
           for avoid \' */
    }
    else if(buffer == 0x27)
    {
        *state = BEGIN;
        printf("%c",buffer);
        /* when buffer is ''', 
           move state to BEGIN
           for end character constant */
    }
    else
    {
        printf("%c",buffer);
        /* buffer isn't specific character,
           keep in character constant */
    }
    return;
}

/*
dfa_character_backslash_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of character string,
when '\' comes, determine is it \'
do action when buffer is '\n', '\'
*/
void dfa_character_backslash_handler(int buffer, 
                                     int* state, 
                                     int* line_cnt)
{
    if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    else if(buffer == 0x5C)
    {
        printf("%c",buffer);
        return;
        /* when buffer is '\', 
           keep state in CHARACTER_BACKSLASH
           for avoid \' */
    }
    else
    {
        printf("%c",buffer);
        /* buffer isn't specific character,
           keep in character constant */
    }
    *state = CHARACTER_CONSTANT;
    /* next of \ isn't ',
       so move state to CHARACTER_CONSTANT*/
    return;
}

/*
dfa_string_constant_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of string,
don't make comment 
do action when buffer is '\n' '\' '""
*/
void dfa_string_constant_handler(int buffer, 
                                 int* state, 
                                 int* line_cnt)
{
    if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    else if(buffer == 0x5C)
    {
        *state = STRING_BACKSLASH;
        printf("%c",buffer);
        /* when buffer is '\', 
           move state to STRING_BACKSLASH
           for avoid \" */
    }
    else if(buffer == '"')
    {
        *state = BEGIN;
        printf("%c",buffer);
        /* when buffer is '"', 
           move state to BEGIN
           for end string */
    }
    else
    {
        printf("%c",buffer);
        /* buffer isn't specific character,
           keep in string */
    }
    return;
}

/*
dfa_string_backslash_handler function
input : value of buffer, address of state, address of line_cnt
return : - (change state's value, add line_cnt's value)
buffer is part of string,
when '\' comes, determine is it \"
do action when buffer is '\n', '\'
*/
void dfa_string_backslash_handler(int buffer, 
                                  int* state, 
                                  int* line_cnt)
{
    if(buffer == '\n')
    {
        increase_line(line_cnt);
        /* when buffer is '\n', 
           add one on line_cnt and newline on stdout */
    }
    else if(buffer == 0x5C)
    {
        printf("%c",buffer);
        return;
        /* when buffer is '\', 
           keep state in STRING_BACKSLASH
           for avoid \" */
    }
    else
    {
        printf("%c",buffer);
        /* buffer isn't specific character,
           keep in character constant */
    }
    *state = STRING_CONSTANT;
    /* next of \ isn't ",
       so move state to STRING_CONSTANT*/
    return;
}

/*
is_dfa_error function
input : address of state
return : 1(end dfa with error), 0(end success)
determine DFA end with error
*/
int is_dfa_error(int* state)
{
    if((*state == COMMENT) || (*state == CHECK_COMMENT_END))
    {
        *state = END_FAILURE;
        return 1;
        /* when finish state is COMMENT or CHECK_COMMENT_END,
            that's mean unterminated comment.
            so, return true */
    }
    return 0;
    /* end success
       return false */
}

/*
decomment_error_handler function
input : value of comment_start_line
return : EXIT_FAILURE
Recording the contents of an error
*/
int decomment_error_handler(int comment_start_line)
{
    fprintf(stderr, 
            "Error: line %d: unterminated comment\n"
            ,comment_start_line);
            /*write contents of error in stderr*/
    return EXIT_FAILURE;
    /* return 1 */
}

/*
decomment_success_handler function
input : address of state
return : EXIT_SUCCESS
move state to END_SUCCESS
*/
int decomment_success_handler(int* state)
{
    *state = END_SUCCESS;
    /* move state to END_SUCCESS */
    return EXIT_SUCCESS;
    /* return 0 */
}