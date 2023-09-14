/* Seungjae Lim / Assignment2 / sgrep.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

/* recursive function for search pattern */
int SearchPattern_In(const char *pattern, const char *buf);

/*
 * Fill out your own functions here (If you need) 
 */

/*
function SearchPattern_In
  Search pattern for recusivly (callee of SeachPattern)
parameter
  const char *pattern - read only string finding string(pattern)
  const char *buf - read only string for find line(part of buf)
return
  int - 1 for success
        0 for fail
*/
int
SearchPattern_In(const char *pattern, const char *buf)
{
  char *ptrPattern, *ptrBuf;
    /*
    ptrPattern - pointing part of pattern
    ptrBuf - pointing part of buf
  */
  ptrPattern = StrFindChr(pattern, '*');
  /* pointing pattern's first '*' */
  if(ptrPattern == NULL)
  {
    /*base case, no more '*' */
    if(StrFindStr(buf,pattern))
    {
      return TRUE;
      /*if pattern is matched, return 1*/
    }
    else
    {
      return FALSE;
      /*no pattern in buf, return 0*/
    }
  }
  else if(*pattern == *ptrPattern)
  {
    /*case pattern start with * (like **)*/
    ptrPattern++;
    return SearchPattern_In(ptrPattern, buf);
    /*recursively search for next pattern*/
  }
  else
  {
    /* recursive case, pattern has '*' */
    *ptrPattern = '\0';
    /*devide pattern '*' for recursive search*/
    ptrPattern++;
    /*ptrPattern pointing after part of devided pattern */
    ptrBuf = StrFindStr(buf,pattern);
    /*search before pattern in line*/
    if(ptrBuf == NULL)
    {
      return FALSE;
      /*if no pattern in line, return 0*/
    }
    ptrBuf = ptrBuf + StrGetLength(pattern);
    /*devide buf for left part*/
    return SearchPattern_In(ptrPattern, ptrBuf);
    /*find pattern in line for recursively
        - (next part of pattern, next part of buf)*/
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
   print out "Error: argument is too long"
   
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
function SearchPattern
  Search pattern in stdin and print line such that have pattern
parameter
  const char *pattern - read only string finding string(pattern)
return
  int - 1 for success
        0 for fail
*/
int
SearchPattern(const char *pattern)
{
  char buf[MAX_STR_LEN + 2];
  /* buffer which have each line of stdin*/
  int len, len_pattern, is_find;
  /*
    len : length of stdin's one line
    len_pattern : length of pattern
    is_find : determine success search
  */
  char *ptrPattern, *ptrBuf, *tmp_pattern;
  /*
    ptrPattern - pointing part of tmp_pattern
    ptrBuf - pointing part of buf
    tmp_pattern - pointing copy of pattern
  */

  /* 
   *  TODO: check if pattern is too long
   */

  /* check the length of a pattern */
  if((len_pattern = StrGetLength(pattern)) > MAX_STR_LEN)
  {
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
    /*error handling for pattern*/
  }
  tmp_pattern = (char *)malloc(len_pattern+1);
  /* make copy of pattern */
  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) {
    /* check the length of an input line */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
      /*error handling for long line*/
    }
    StrCopy(tmp_pattern, pattern);
    /* copy pattern to tmp_pattern for edit pattern*/
    ptrPattern = StrFindChr(tmp_pattern, '*');
    /* pointing pattern's first '*' */
    if(ptrPattern == NULL)
    {
      /*if no '*' in pattern*/
      if(StrFindStr(buf,tmp_pattern))
      {
        printf("%s", buf);
        /*if pattern is matched, print line*/
      }
    }
    else
    {
      /* pattern has '*' */
      *ptrPattern = '\0';
      /*devide tmp_pattern '*' for recursive search*/
      ptrPattern++;
      /*ptrPattern pointing after part of devided pattern */
      ptrBuf = StrFindStr(buf,tmp_pattern);
      /*search before pattern in line*/
      if(ptrBuf == NULL)
      {
        continue;
        /*if no pattern in line, search next line*/
      }
      ptrBuf = ptrBuf + StrGetLength(tmp_pattern);
      /*devide buf for left part*/
      is_find = SearchPattern_In(ptrPattern, ptrBuf);
      /*find pattern in line for recursively
        - (next part of pattern, next part of buf)*/
      if(is_find != 0 )
      {
        printf("%s", buf);
        /* if pattern is matched, print line */
      }
      else
      {
        continue;
        /*if no pattern in line, search next line*/
      }
    }
  }
  return TRUE;
  /* search end success*/
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