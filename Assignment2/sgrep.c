//20210251 jaehyun park, EE209 assignment 2, sgrep.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

/*
FUNCTION: SearchPatternStar
1. Get the character pointer of the pattern and the buffer
2. Split the pattern into two parts by star
3. Search the first part of the pattern in the buffer by
  recursively calling SearchPatternStar
4. Search the second part of the pattern in the buffer by
  recursively calling SearchPatternStar
5.

*/
int
SearchPatternStar(char* pattern, char* buf){
  // patternTmp is used to store the copy of the pattern
  char patternTmp[MAX_STR_LEN + 2];

  // I split the pattern into two parts by star
  // and store the first part in patternTmp1
  // and store the second part in patternTmp2
  char patternTmp1[MAX_STR_LEN + 2];
  char patternTmp2[MAX_STR_LEN + 2];

  // bufTmp is used to store some part of the buffer
  // It is used to store the portion after first part appears
  char bufTmp[MAX_STR_LEN + 2];

  char* pt;
  int n;

  // Search the star in the pattern and split it by null character
  StrCopy(patternTmp, pattern);
  pt = StrFindChr(patternTmp, '*');
  *pt = '\0';

  // Store each part of the pattern in patternTmp1 and patternTmp2
  StrCopy(patternTmp1, patternTmp);
  StrCopy(patternTmp2, pt+1);

  // If pt is not null, buf contains the first part of the pattern
  pt = StrFindStr(buf, patternTmp1);
  n = StrGetLength(patternTmp1);

  // Pt is not null and the first part of the pattern is not empty
  if(pt && n!=0){
    // Store the portion after first part appears in bufTmp
    pt += n;
    StrCopy(bufTmp, pt);

    // The second part of the pattern has star, 
    // recursively call SearchPatternStar
    if(StrFindChr(patternTmp2,'*')){
      return SearchPatternStar(patternTmp2, bufTmp);
    }

    // The second part of the patter does not have star,
    else{
      // Buffer contains the second part of the pattern
      if(StrFindStr(bufTmp, patternTmp2)){
        return TRUE;
      }
      // Buffer does not contain the second part of the pattern
      else{
        return FALSE;
      }
    }
  }

  // Pt is not null and the first part of the pattern is empty
  else if(pt && n==0){

    // The second part of the pattern has star,
    // recursively call SearchPatternStar
    if(StrFindChr(patternTmp2,'*')){
      return SearchPatternStar(patternTmp2, buf);
    }

    // The second part of the pattern does not have star
    else{
      // Buffer contains the second part of the pattern
      if(StrFindStr(buf, patternTmp2)){
        return TRUE;
      }
      // Buffer does not contain the second part of the pattern
      else{
        return FALSE;
      }
    }
  }

  // If pt is null, buf does not contain the first part of the pattern
  else{
    return FALSE;
  }
}
  
/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void
PrintUsage(const char* argv0)
{
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* SearchPattern()
   Your task:
   1. Do argument validation
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long,
   print out "Error: pattern is too long"

   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes,
   print out "Error: input line is too long"
   - Error message should be printed out to standard error (stderr)

   3. Check & print out the line contains a given string (search-string)

   Tips:
   - fgets() is an useful function to read characters from file. Note
   that the fget() reads until newline or the end-of-file is reached.
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/

/*
FUNCTION: SearchPattern
1. Get the character pointer of the pattern
2. Read the line from stdin
3. If the pattern is found in the line, print out the line
4. If the pattern is not found in the line, do nothing
5. Read the next line from stdin and repeat step 3 and 4
6. If the end of the file is reached, return TRUE
*/

int
SearchPattern(const char *pattern)
{
  char buf[MAX_STR_LEN + 2];
  
  // If the pattern is longer than 1023, print error into stderr
  // and return FALSE
  if(strlen(pattern) > MAX_STR_LEN) {
    fprintf(stderr, "Error: pattern is too long\n");
    return FALSE;
  }

  // Read one line at a time from stdin, and process each line
  while (fgets(buf, sizeof(buf), stdin)) {

    // If the line is longer than 1023, print error into stderr
    // and return FALSE
    if(strlen(buf) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    
    // If the pattern has a star, call SearchPatternStar
    if(StrFindChr(pattern, '*')) {
      if(SearchPatternStar((char*)pattern, buf)) {
        // If the pattern is founded in the line, print out the line
        printf("%s", buf);
      }
    }

    // If the pattern does not have a star, call StrFindStr
    else {
      if(StrFindStr(buf, pattern)) {
        // If the pattern is founded in the line, print out the line
        printf("%s", buf); 
      }
    }
  }

  return TRUE; // If there is no problem, return TRUE
}
/*-------------------------------------------------------------------*/
int
main(const int argc, const char *argv[])
{
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}
