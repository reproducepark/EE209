//20210251 jaehyun park, EE209 assignment 3, customer_manager1.c
//Implementation of customer_manager API by using array of structures

#include <assert.h> // for assert()
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "customer_manager.h" // customer_manager API

#define UNIT_ARRAY_SIZE 1024 // unit size of array

// structure of customer user information
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

// sturcture of customer DB
struct DB {
  struct UserInfo *pArray;    // pointer to the array
  int curArrSize;             // current array size (max # of elements)
  int numItems;               // # of stored items, needed to determine
                              // # whether the array should be expanded
                              // # or not
                              // also, index of the next empty space
};

/*--------------------------------------------------------------------*/
/*
Function: CreateCustomerDB
Input: none
Output: pointer to the DB structure

1. Allocate memory for the DB structure
2. Initialize the DB structure
3. If memory allocation fails, print error message to stderr and return
NULL
*/

DB_T
CreateCustomerDB(void)
{

  DB_T d;
  
  d = (DB_T) malloc(sizeof(struct DB));

  // handle memory allocation failure
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->numItems = 0; // initially no items in the array
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));

  // handle memory allocation failure
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }

  return d;

}
/*--------------------------------------------------------------------*/
/*
Function: DestroyCustomerDB
Input: pointer to the DB structure
Output: none

1. traverse the array and free the memory allocated for each element
2. free the memory allocated for the array
3. free the memory allocated for the DB structure
*/

void
DestroyCustomerDB(DB_T d)
{
  if (d != NULL) {
    assert(d->numItems >= 0);
    if (d->pArray != NULL) {
      for (int i = 0; i < d->numItems; i++) {
        free(d->pArray[i].name); // free name
        free(d->pArray[i].id); // free id
      }
      free(d->pArray); // free the array
    }
    free(d); // free the DB structure
  }
  return;

}
/*--------------------------------------------------------------------*/
/*
Function: RegisterCustomer
Input: pointer to the DB structure, customer id, customer name, and
purchase amount
Output: 0 if successful, -1 if failed

1. Check if the id, name already exists in the DB
2. Add the new customer information to the DB
3. If the array is full, expand the array
4. If memory allocation fails, print error message to stderr and return
-1
*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  //check if the d, id, name is null, check if the purchase is 
  //non-positive
  if (d == NULL || id == NULL || name == NULL || purchase <= 0) {
    return -1;
  }
  assert(d->pArray);
  assert(d->numItems >= 0);
  assert(d->curArrSize > 0);

  //check if the id or name is already in the array
  for (int i = 0; i < d->numItems; i++) {
    if (strcmp(d->pArray[i].id, id) == 0 || strcmp(d->pArray[i].name,
    name) == 0) {
      return -1;
    }
  }

  //add the new customer
  d->pArray[d->numItems].name = (char *)malloc(strlen(name) + 1);
  // handle memory allocation failure
  if(d->pArray[d->numItems].name == NULL) {
    fprintf(stderr, "Can't allocate a memory for name");
    return -1;
  }
  strcpy(d->pArray[d->numItems].name, name);

  d->pArray[d->numItems].id = (char *)malloc(strlen(id) + 1);
  // handle memory allocation failure
  if(d->pArray[d->numItems].id == NULL) {
    fprintf(stderr, "Can't allocate a memory for id");
    return -1;
  }
  strcpy(d->pArray[d->numItems].id, id);

  d->pArray[d->numItems].purchase = purchase;
  d->numItems++;

  //check if the array is full
  //if it is full, expand the array by UNIT_ARRAY_SIZE
  if (d->numItems == d->curArrSize) {
    d->curArrSize += UNIT_ARRAY_SIZE;
    d->pArray = (struct UserInfo *)realloc(d->pArray, 
      d->curArrSize * sizeof(struct UserInfo));
    // handle memory allocation failure
    if (d->pArray == NULL) {
      fprintf(stderr, "Can't allocate a memory for array of size %d\n",
        d->curArrSize);
      return -1;
    }
  }

  return 0;
}
/*--------------------------------------------------------------------*/
/*
Function: UnregisterCustomerByID
Input: pointer to the DB structure, customer id
Output: 0 if successful, -1 if failed

1. Find the customer information with the given id
2. Remove the customer information from the array
3. Move remaining elements to fill the gap
*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  //check if the d, id is null
  if (d == NULL || id == NULL) {
    return -1;
  }

  assert(d->pArray);
  assert(d->numItems >= 0);

  //find the customer information with the given id
  for (int i = 0; i < d->numItems; i++) {
    if (strcmp(d->pArray[i].id, id) == 0) {
      //remove the customer
      free(d->pArray[i].name);
      free(d->pArray[i].id);
      //move the remaining elements to fill the gap
      for (int j = i; j < d->numItems - 1; j++) {
        d->pArray[j] = d->pArray[j + 1];
      }
      d->numItems--;
      assert(d->numItems >= 0);
      //set previous last element empty
      d->pArray[d->numItems].name = NULL;
      d->pArray[d->numItems].id = NULL;
      d->pArray[d->numItems].purchase = 0;
      return 0;
    }
  }

  //if the id is not found
  return -1;
}

/*--------------------------------------------------------------------*/
/*
Function: UnregisterCustomerByName
Input: pointer to the DB structure, customer name
Output: 0 if successful, -1 if failed

1. Find the customer information with the given name
2. Remove the customer information from the array
3. Move remaining elements to fill the gap
*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  //check if the d, name is null
  if (d == NULL || name == NULL) {
    return -1;
  }

  assert(d->pArray);
  assert(d->numItems >= 0);

  //find the customer information with the given name
  for (int i = 0; i < d->numItems; i++) {
    if (strcmp(d->pArray[i].name, name) == 0) {
      //remove the customer
      free(d->pArray[i].name);
      free(d->pArray[i].id);
      //move the remaining elements to fill the gap
      for (int j = i; j < d->numItems - 1; j++) {
        d->pArray[j] = d->pArray[j + 1];
      }
      d->numItems--;
      assert(d->numItems >= 0);
      //set previous last element empty
      d->pArray[d->numItems].name = NULL;
      d->pArray[d->numItems].id = NULL;
      d->pArray[d->numItems].purchase = 0;
      return 0;
    }
  }

  //if the name is not found
  return -1;
}
/*--------------------------------------------------------------------*/
/*
Function: GetPurchaseByID
Input: pointer to the DB structure, customer id
Output: purchase amount if successful, -1 if failed

1. Find the customer information with the given id
2. Return the purchase amount
*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  //check if the d, id is null
  if (d == NULL || id == NULL) {
    return -1;
  }

  assert(d->pArray);
  assert(d->numItems >= 0);

  //find the customer information with the given id
  for (int i = 0; i < d->numItems; i++) {
    if (strcmp(d->pArray[i].id, id) == 0) {
      return d->pArray[i].purchase;
    }
  }

  //if the id is not found
  return -1;
}
/*--------------------------------------------------------------------*/
/*
Function: GetPurchaseByName
Input: pointer to the DB structure, customer name
Output: purchase amount if successful, -1 if failed

1. Find the customer information with the given name
2. Return the purchase amount
*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  //check if the d, name is null
  if (d == NULL || name == NULL) {
    return -1;
  }

  assert(d->pArray);
  assert(d->numItems >= 0);

  //find the customer information with the given name
  for (int i = 0; i < d->numItems; i++) {
    if (strcmp(d->pArray[i].name, name) == 0) {
      return d->pArray[i].purchase;
    }
  }

  //if the name is not found
  return -1;
}
/*--------------------------------------------------------------------*/
/*
Function: GetSumCustomerPurchase
Input: pointer to the DB structure, function pointer
Output: sum of purchase amount if successful, -1 if failed

1. Traverse the array and call the function pointer for each customer
2. Return the sum of purchase amount
*/

int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  //check if the d, fp is null
  if (d == NULL || fp == NULL) {
    return -1;
  }

  int sum = 0;

  assert(d->pArray);
  assert(d->numItems >= 0);

  //traverse the array and call the function pointer for each customer
  for (int i = 0; i < d->numItems; i++) {
    sum += fp(d->pArray[i].id, d->pArray[i].name, 
    d->pArray[i].purchase);
  }
  return sum;
}
