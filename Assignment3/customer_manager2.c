//20210251 jaehyun park, EE209 assignment 3, customer_manager1.c
//Implementation of customer_manager API by using hash table and 
//doubly linked list

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_BUCKET_SIZE 1024
#define MAX_BUCKET_SIZE 1048576
enum {HASH_MULTIPLIER = 65599};

// structure of customer user information
// use doubly linked list to maintain two hash tables easily
struct UserInfo {
  char *name;                     // customer name
  char *id;                       // customer id
  int purchase;                   // purchase amount (> 0)
  struct UserInfo *nextById;      // next node by id
  struct UserInfo *nextByName;    // next node by name
  struct UserInfo *prevById;      // previous node by id to support
                                  // doubly linked list
  struct UserInfo *prevByName;    // previous node by name to support
                                  // doubly linked list
};

struct DB {
  struct UserInfo **tableById;    // hashtable stores by id
  struct UserInfo **tableByName;  // hashtable stores by name
  int curBucketSize;              // current bucket size of hashtables
  int numItems;                   // # of stored items, needed to 
                                  // determine whether the hashtable  
                                  // should be expanded or not
};
//functions for linked list


/*--------------------------------------------------------------------*/
/* 
Function: Hash_Function
Input: string, Bucket size
Output: hash value

1.Return a hash code for pcKey that is between 0 and iBucketCount-1,
  inclusive. 

Adapted from the EE209 lecture notes. 
*/
static int Hash_Function(const char *pcKey, int iBucketCount);

/*--------------------------------------------------------------------*/
/*
Function: AddNode
Input: pointer to two hash tables (by id and by name), pointer to the 
       node to be added, current bucket size
Output: none

1. Get hashcode of the node to be added
2. Add the node to the two hash tables
*/
void AddNode(struct UserInfo **tableById, 
struct UserInfo **tableByName, struct UserInfo *p, int curBucketSize);

/*--------------------------------------------------------------------*/
/*
Function: RemoveNode
Input: pointer to two hash tables (by id and by name), pointer to the 
       node to be removed, current bucket size
Output: none

1. Get hashcode of the node to be removed
2. Remove the node from the two hash tables
*/
void RemoveNode(struct UserInfo **tableById,
struct UserInfo **tableByName, struct UserInfo *p, int curBucketSize);

/*--------------------------------------------------------------------*/
/*
Function: FreeTable
Input: pointer to table, bucket size
Output: none

1. Traverse the table and free all the nodes
2. Free the table
*/
void FreeTable(struct UserInfo **table, int BucketSize);

/*--------------------------------------------------------------------*/
/*
Function: ExpandTable
Input: pointer to customer DB
Output: 0 if success, -1 if failure

1. Allocate memory for the new table
2. Traverse the old table and add the nodes to the new table. In this 
   process, we allocate memory for the new nodes.
3. Free the old table
4. If memory allocation fails, print error message to stderr and return
   -1
*/

int ExpandTable(DB_T d);

/*--------------------------------------------------------------------*/
/*
Function: CreateCustomerDB
Input: none
Output: pointer to the DB structure

1. Allocate memory for the DB structure including two hash tables
2. Initialize the DB structure
3. If memory allocation fails, print error message to stderr and return
NULL
*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curBucketSize = UNIT_BUCKET_SIZE; // start with 1024 elements

  d->tableById = (struct UserInfo **)calloc(d->curBucketSize,
               sizeof(struct UserInfo *));
  // handle memory allocation failure
  if (d->tableById == NULL) {
    fprintf(stderr, "Can't allocate a memory for table of size %d\n",
      d->curBucketSize);   
    free(d);
    return NULL;
  }

  d->tableByName = (struct UserInfo **)calloc(d->curBucketSize,
               sizeof(struct UserInfo *));
  // handle memory allocation failure
  if (d->tableByName == NULL) {
    fprintf(stderr, "Can't allocate a memory for table of size %d\n",
      d->curBucketSize);   
    free(d->tableById);
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

1. Free all the nodes and table with FreeTable function
2. Free remain table
3. Free DB structure
*/
void
DestroyCustomerDB(DB_T d)
{
  if (d != NULL) {
    assert(d->curBucketSize > 0);
    if (d->tableById != NULL) {
      FreeTable(d->tableById, d->curBucketSize);
    }
    if (d->tableByName != NULL) {
      free(d->tableByName);
    }
    free(d);
  }
  return;
}
/*--------------------------------------------------------------------*/
/*
Function: RegisterCustomer
Input: pointer to the DB structure, customer name, customer id, 
       purchase amount
Output: 0 if success, -1 if failure

1. Check whether the customer id is already registered by 
   GetCustomerByID and GetCustomerByName
2. Add the new customer information to two hash tables
3. If number of items is bigger than 3/4 of the bucket size, expand the
   table by calling ExpandTable function
4. If memory allocation fails, print error message to stderr and return
-1
*/
int
RegisterCustomer(DB_T d, const char *id,
const char *name, const int purchase)
{
  //check if the d, id, name is null, check if the purchase is
  // non-positive
  if (d == NULL || id == NULL || name == NULL || purchase <= 0) {
    return -1;
  }

  assert(d->tableById);
  assert(d->tableByName);
  assert(d->curBucketSize > 0);
  assert(d->numItems >= 0);

  //check if the id or name is already in the table
  //GetPurchaseID/Name returns -1 if the id or name is not in the table
  if(GetPurchaseByID(d, id) != -1 || GetPurchaseByName(d, name) != -1){
    return -1;
  }

  //add the new element to the table
  struct UserInfo *p= (struct UserInfo*)malloc(sizeof(struct UserInfo));
  // handle memory allocation failure
  if (p == NULL) {
    fprintf(stderr, "Can't allocate a memory for UserInfo\n");
    return -1;
  }

  p->id = (char *)malloc(strlen(id) + 1);
  // handle memory allocation failure
  if (p->id == NULL) {
    fprintf(stderr, "Can't allocate a memory for id\n");
    free(p);
    return -1;
  }
  strcpy(p->id, id);

  p->name = (char *)malloc(strlen(name) + 1);
  // handle memory allocation failure
  if (p->name == NULL) {
    fprintf(stderr, "Can't allocate a memory for name\n");
    free(p->id);
    free(p);
    return -1;
  }
  strcpy(p->name, name);

  p->purchase = purchase;

  //add the new node to the tables
  AddNode(d->tableById, d->tableByName, p, d->curBucketSize);

  d->numItems++;

  //if current bucket size is bigger than MAX_BUCKET_SIZE, do not 
  if (d->curBucketSize >= MAX_BUCKET_SIZE) {
    return 0;
  }
  //if number of items are more than 75% of the bucket size, expand
  //the table
  if (d->numItems >= (d->curBucketSize * 3 / 4)) {
    return ExpandTable(d);
  }
  return 0;
}
/*--------------------------------------------------------------------*/
/*
Function: UnregisterCustomerByID
Input: pointer to the DB structure, customer id
Output: 0 if successful, -1 if failed

1. Find the customer information (node) with the given id
2. Remove the node from two hash tables by calling RemoveNode function
*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  //check if the d, id is null
  if (d == NULL || id == NULL) {
    return -1;
  }

  assert(d->tableById);
  assert(d->tableByName);
  assert(d->curBucketSize > 0);
  assert(d->numItems >= 0);

  int hashForId = Hash_Function(id, d->curBucketSize);
  struct UserInfo *p;

  //find the node with the given id
  for(p = d->tableById[hashForId]; p != NULL; p = p->nextById) {
    if (strcmp(p->id, id) == 0) {
      //remove the node from the table
      RemoveNode(d->tableById, d->tableByName, p, d->curBucketSize);
      d->numItems--;
      assert(d->numItems >= 0);
      return 0;
    }
  }
  return -1;
}

/*--------------------------------------------------------------------*/
/*
Function: UnregisterCustomerByName
Input: pointer to the DB structure, customer name
Output: 0 if successful, -1 if failed

1. Find the customer information (node) with the given name
2. Remove the node from two hash tables by calling RemoveNode function
*/

int
UnregisterCustomerByName(DB_T d, const char *name)
{
  //check if the d, name is null
  if (d == NULL || name == NULL) {
    return -1;
  }

  assert(d->tableById);
  assert(d->tableByName);
  assert(d->curBucketSize > 0);
  assert(d->numItems >= 0);

  int hashForName = Hash_Function(name, d->curBucketSize);
  struct UserInfo *p;

  //find the node with the given name
  for (p = d->tableByName[hashForName]; p != NULL; p = p->nextByName) {
    if (strcmp(p->name, name) == 0) {
      //remove the node from the table
      RemoveNode(d->tableById, d->tableByName, p, d->curBucketSize);
      d->numItems--;
      assert(d->numItems >= 0);
      return 0;
    }
  }
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

  assert(d->tableById);
  assert(d->curBucketSize > 0);

  int hashForId = Hash_Function(id, d->curBucketSize);
  struct UserInfo *p;

  //find the node with the given id
  for (p = d->tableById[hashForId]; p != NULL; p = p->nextById) {
    if (strcmp(p->id, id) == 0) {
      return p->purchase;
    }
  }

  //if the id is not in the table, return -1
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

  assert(d->tableByName);
  assert(d->curBucketSize > 0);

  int hashForName = Hash_Function(name, d->curBucketSize);
  struct UserInfo *p;

  //find the node with the given name
  for (p = d->tableByName[hashForName]; p != NULL; p = p->nextByName) {
    if (strcmp(p->name, name) == 0) {
      return p->purchase;
    }
  }
  
  //if the name is not in the table, return -1
  return -1;
}
/*--------------------------------------------------------------------*/
/*
Function: GetSumCustomerPurchase
Input: pointer to the DB structure, function pointer
Output: sum of purchase amount if successful, -1 if failed

1. Traverse the table and call the function pointer for each customer
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
  struct UserInfo *p;

  assert(d->tableById);
  assert(d->curBucketSize > 0);

  //traverse the table and call the function pointer for each customer
  for(int i = 0; i < d->curBucketSize; i++) {
    for(p = d->tableById[i]; p != NULL; p = p->nextById) {
      sum+=fp(p->id, p->name, p->purchase);
    }
  }
  return sum;
}
/*--------------------------------------------------------------------*/
static int Hash_Function(const char *pcKey, int iBucketCount)
{
  assert(pcKey);
  assert(iBucketCount > 0);

  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
          + (unsigned int)pcKey[i];
  return (int)(uiHash % (unsigned int)iBucketCount);
}
/*--------------------------------------------------------------------*/
void AddNode(struct UserInfo **tableById, 
struct UserInfo **tableByName, struct UserInfo *p, int curBucketSize) {
  assert(tableById);
  assert(tableByName);
  assert(p);
  assert(curBucketSize > 0);

  int hashForId = Hash_Function(p->id, curBucketSize);
  int hashForName = Hash_Function(p->name, curBucketSize);

  p->nextById = tableById[hashForId];
  if(tableById[hashForId] != NULL) {
    tableById[hashForId]->prevById = p;
  }
  p->prevById = NULL;
  tableById[hashForId] = p;

  p->nextByName = tableByName[hashForName];
  if(tableByName[hashForName] != NULL) {
    tableByName[hashForName]->prevByName = p;
  }
  p->prevByName = NULL;
  tableByName[hashForName] = p;
}
/*--------------------------------------------------------------------*/
void RemoveNode(struct UserInfo **tableById,
struct UserInfo **tableByName, struct UserInfo *p, int curBucketSize) {
  assert(tableById);
  assert(tableByName);
  assert(p);
  assert(curBucketSize > 0);
  
  int hashForId = Hash_Function(p->id, curBucketSize);
  int hashForName = Hash_Function(p->name, curBucketSize);

  if(p->prevById == NULL){
    tableById[hashForId] = p->nextById;
  }
  else{
    p->prevById->nextById = p->nextById;
  }
  if(p->prevByName == NULL){
    tableByName[hashForName] = p->nextByName;
  }
  else{
    p->prevByName->nextByName = p->nextByName;
  }
  if(p->nextById!=NULL){
    p->nextById->prevById = p->prevById;
  }
  if(p->nextByName!=NULL){
    p->nextByName->prevByName = p->prevByName;
  }
  //since we owned the id and name, we need to free them
  free(p->id);
  free(p->name);
  free(p);
}
/*--------------------------------------------------------------------*/
void FreeTable(struct UserInfo **table, int BucketSize) {
  assert(table);
  assert(BucketSize > 0);

  struct UserInfo *p, *next;

  //free the nodes first
  for (int i = 0; i < BucketSize; i++) {
    for (p = table[i]; p != NULL; p = next) {
      next = p->nextById;
      free(p->name);
      free(p->id);
      free(p);
    }
  }
  //free the table
  free(table);
}

int ExpandTable(DB_T d){
  assert(d);
  assert(d->curBucketSize > 0);
  assert(d->tableById);
  assert(d->tableByName);

  int PrevBucketSize = d->curBucketSize;
  struct UserInfo *p;

  //double the bucket size
  d->curBucketSize *= 2;

  //create a new table
  struct UserInfo **newTableById = (struct UserInfo **)calloc(
    d->curBucketSize,sizeof(struct UserInfo *));
  // handle memory allocation failure
  if (newTableById == NULL) {
    fprintf(stderr, "Can't allocate a memory for table of size %d\n",
      d->curBucketSize);   
    return -1;
  }

  struct UserInfo **newTableByName = (struct UserInfo **)calloc(
    d->curBucketSize,sizeof(struct UserInfo *));
  // handle memory allocation failure
  if (newTableByName == NULL) {
    fprintf(stderr, "Can't allocate a memory for table of size %d\n",
      d->curBucketSize);   
    return -1;
  }

  //traverse the old table and add the nodes to the new table
  for(int i = 0; i < PrevBucketSize; i++) {
    for(p = d->tableById[i]; p != NULL; p = p->nextById) {
      //create a new node and copy the id, name, and purchase
      struct UserInfo *newP = (struct UserInfo *)malloc(
        sizeof(struct UserInfo));
      // handle memory allocation failure
      if (newP == NULL) {
        fprintf(stderr, "Can't allocate a memory for UserInfo\n");
        return -1;
      }
      newP->id = (char *)malloc(strlen(p->id) + 1);
      // handle memory allocation failure
      if (newP->id == NULL) {
        fprintf(stderr, "Can't allocate a memory for id\n");
        free(newP);
        return -1;
      }
      strcpy(newP->id, p->id);
      newP->name = (char *)malloc(strlen(p->name) + 1);
      // handle memory allocation failure
      if (newP->name == NULL) {
        fprintf(stderr, "Can't allocate a memory for name\n");
        free(newP->id);
        free(newP);
        return -1;
      }
      strcpy(newP->name, p->name);
      newP->purchase = p->purchase;
      
      // add the new node to the new table by AddNode function
      AddNode(newTableById, newTableByName, newP, d->curBucketSize);
    }
  }
  //free the old table
  FreeTable(d->tableById, PrevBucketSize);
  free(d->tableByName);

  //update the pointer
  d->tableById = newTableById;
  d->tableByName = newTableByName;
  return 0;
}