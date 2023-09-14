//20210251 jaehyun park, EE209 assignment 2, str.c

//included libraries
#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include <strings.h>
#include <ctype.h> /* for isdigit() and isspace() */
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*--------------------------------------------------------------------*/

/*
Function : StrGetLength
1. Get the character pointer as input
2. Count the number of characters until the null character
3. Return the number of characters
*/

size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd; // pointer to the end of the string

  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */

  pcEnd = pcSrc; // initialize the pointer to the end of the string
	
  // find the end of the string by while loop
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc); // return the number of characters
}

/*--------------------------------------------------------------------*/

/*
Function : StrCopy
1. Get two character pointers as input
2. Copy the string from the source string to the destination string
3. Return the beginning of the destination string
*/

char *StrCopy(char *pcDest, const char* pcSrc)
{
  char *pcDestTmp; // temporary pointer that traverse the destination
  const char *pcSrcTmp; // temporary pointer that traverse the source

  assert(pcDest); // to check if the destination pointer is NULL
  assert(pcSrc); // to check if the source pointer is NULL

  pcDestTmp = pcDest;  // initialize the temporary pointer
                        // to the beginning of the destination string
  pcSrcTmp = pcSrc;    // initialize the temporary pointer 
                        // to the beginning of the source string

  // copy the string until the null character
  while(*pcSrcTmp){ 
    *pcDestTmp = *pcSrcTmp; 
    pcDestTmp++;
    pcSrcTmp++;
  }

  *pcDestTmp = '\0'; // add the null character at the end of the string

  return pcDest; // return the beginning of the destination string
}

/*--------------------------------------------------------------------*/

/*
Function : StrCompare
1. Get two character pointers as input
2. Compare the two strings
3. Return the difference between the two strings
*/

int StrCompare(const char* pcS1, const char* pcS2)
{
  // temporary pointers that traverse the strings
  const unsigned char *pcS1Tmp; 
  const unsigned char *pcS2Tmp;

  // to check if the pointers are NULL
  assert(pcS1);
  assert(pcS2);

  // initialize the temporary pointers to the beginning of each strings
  pcS1Tmp = (const unsigned char*) pcS1; 
  pcS2Tmp = (const unsigned char*) pcS2;

  // compare the strings until the null character
  while(*pcS1Tmp && *pcS2Tmp){
    if (*pcS1Tmp != *pcS2Tmp){ // if the characters are different
      return *pcS1Tmp - *pcS2Tmp; // return the difference
    }
    pcS1Tmp++;
    pcS2Tmp++;
  }

  //if the strings are the same until the null character
  //note that one of the strings may be longer than the other
  return *pcS1Tmp - *pcS2Tmp; // return the difference
 
}
/*--------------------------------------------------------------------*/

/*
Function : StrFindChar
1. Get character pointer and one character as input
2. Find the character in the string
3. Return the position of the character in the string or NULL
*/ 

char *StrFindChr(const char* pcHaystack, int c)
{
  const char *pcTmp; // temporary pointer that traverse the string

  assert(pcHaystack); // to check if the pointer is NULL

  pcTmp = pcHaystack; // initialize the pointer to the beginning

  // traverse the string until the null character
  while(*pcTmp){
    if (*pcTmp == c){ // if the character is found
      return (char*)pcTmp; // return the position of the character
    }
    pcTmp++;
  }

  if(*pcTmp == c){ // if the character is null character
    return (char*)pcTmp; // return the position of the character
  }
  else{ // if the character is not found
    return NULL;  // return NULL
  }

}
/*--------------------------------------------------------------------*/

/*
Function : StrFindStr
1. Get two character pointers as input
2. Find the second string in the first string
3. Return the starting position of the string or NULL
*/

char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  // temporary pointers that traverse the strings
  const char *pcTmp; 
  const char *pcTmp2;
  const char *result;

  // to check if the pointers are NULL
  assert(pcHaystack);
  assert(pcNeedle);

  // initialize the temporary pointers to the beginning of each strings
  pcTmp = pcHaystack;
  pcTmp2 = pcNeedle;

  // if the second string is null character
  if(*pcNeedle == '\0'){
    return (char*)pcHaystack; //return the beginning of the first string
  }

  // nested while loop to find the second string in the first string
  while(*pcTmp){ //true if not null
    if (*pcTmp == *pcTmp2){ //check if the first character is same
      result = pcTmp; //save the address of first character

      //check if the rest of the characters are same
      while(*pcTmp2 && *pcTmp && *pcTmp == *pcTmp2){
        pcTmp++; 
        pcTmp2++;
      }

      // if the second string is found
      if(*pcTmp2 == '\0'){
        return (char*)result; // return the starting position
      }

      // if the second string is not found
      // if it is end of the first string
      else if(*pcTmp == '\0'){
        return NULL; // return NULL
      }
      // if it is not end of the first string
      else{
        pcTmp = result;
        pcTmp2 = pcNeedle;
      }
    }
    pcTmp++;
  }
  
  // case that the first string is shorter than the second string
  return NULL; // return NULL
}
/*--------------------------------------------------------------------*/

/*
Function : StrToLong
1. Get two character pointers as input
2. Concatenate the source string to the destination string
3. Return the beginning of the destination string
*/

char *StrConcat(char *pcDest, const char* pcSrc)
{
  // temporary pointers that traverse the strings
  char *pcDestTmp;
  const char *pcSrcTmp;

  // to check if the pointers are NULL
  assert(pcDest);
  assert(pcSrc);

  // initialize the temporary pointers to the beginning of each strings
  pcDestTmp = pcDest;
  pcSrcTmp = pcSrc;

  // traverse the destination string until the null character
  while(*pcDestTmp){
    pcDestTmp++;
  }

  // concatenate the source string to the destination string
  while(*pcSrcTmp){
    *pcDestTmp = *pcSrcTmp;
    pcDestTmp++;
    pcSrcTmp++;
  }

  // add the null character at the end of the string
  *pcDestTmp = '\0';

  // return the beginning of the destination string
  return pcDest;
}

/*--------------------------------------------------------------------*/

/*
Function : StrToLong
1.Get character pointer and double pointer, and one integer as input
2.Convert the string to long integer
3.If endptr is not NULL, store the address of the first invalid character
4.Return the converted integer
*/

long int StrToLong(const char *nptr, char **endptr, int base)
{
  // I used unsigned long int to store the converted integer
  // First, I store the absolute value of the integer
  // Then, I multiply the sign to the absolute value
  // To handle negative overflow
  unsigned long int result = 0;

  // temporary pointer that traverse the string
  const char *pcTmp;

  // to check if nothing to convert
  const char *pcSave;

  // to check if the number is negative
  int chkNegative = 0;

  assert(nptr);

  pcTmp = nptr;
  
  /* handle only when base is 10 */
  if (base != 10) return 0;

  //skip white space characters
  while(*pcTmp&&isspace(*pcTmp)){
    pcTmp++;
  }

  //if nothing to convert
  if(*pcTmp == '\0'){ 
    if(endptr){ //if endptr is not NULL we should store the address
    *endptr = (char*)(nptr); //store the beginning of the string
    }
    return 0L; //return 0
  }

  //check if it is negative or positive
  else if(*pcTmp == '-'){
    chkNegative = 1;
    pcTmp++;
  }
  else if(*pcTmp == '+'){
    pcTmp++;
  }
  
  //save the position of Tmp to check whether it is changed or not
  pcSave = pcTmp;
  
  //convert the string to long integer
  while(isdigit(*pcTmp)&&*pcTmp){
    result = result * 10 + (*pcTmp - '0');
    pcTmp++;
  }

  //if nothing to convert
  if(pcSave == pcTmp){
    if(endptr){ //if endptr is not NULL we should store the address
    *endptr = (char*)(nptr); //store the beginning of the string
    }
    return 0L; //return 0
  }

  // we should handle the overflow
  else{

    if(endptr){ //if endptr is not NULL we should store the address
    *endptr = (char*)(pcTmp); //store the address of first invalid char
    }

    //if it is negative
    if(chkNegative){
      //if it is overflow
      if(result < 0){ 
        return LONG_MIN;
      }
      else if(-result < LONG_MIN){
        return LONG_MIN;
      }

      //if it is not overflow
      else{
        return -result;
      }
    }

    //if it is positive
    else{
      //if it is overflow
      if(result < 0){
        return LONG_MAX;
      }
      else if(result > LONG_MAX){
        return LONG_MAX;
      }

      //if it is not overflow
      else{
        return result;
      }
    }
  }
}

/*------------------------------------------------------------------------*/
int StrCaseCompare(const char *pcS1, const char *pcS2)
{
  //temporary pointers that traverse the strings
  const unsigned char *pcS1Tmp;
  const unsigned char *pcS2Tmp;

  //to check if the pointers are NULL
  assert(pcS1);
  assert(pcS2);

  //initialize the temporary pointers to the beginning of each strings
  pcS1Tmp = (const unsigned char*)pcS1;
  pcS2Tmp = (const unsigned char*)pcS2;

  // compare the strings until the null character
  while(*pcS1Tmp && *pcS2Tmp){
    if (tolower(*pcS1Tmp) != tolower(*pcS2Tmp)){ //if they are different
      return tolower(*pcS1Tmp) - tolower(*pcS2Tmp); //return the diff
    }
    pcS1Tmp++;
    pcS2Tmp++;
  }

  //if the strings are same until the null character
  //note that one of the strings may be longer than the other
  return tolower(*pcS1Tmp) - tolower(*pcS2Tmp); //return the diff

}
