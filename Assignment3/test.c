#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};
//24 bytes

struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};
//16 bytes

typedef struct DB* DB_T;

int main(){
    DB_T d;
    d = (DB_T) calloc(1, sizeof(struct DB));
    d->curArrSize = 10;
    d->pArray = (struct UserInfo *)calloc(d->curArrSize, sizeof(struct UserInfo));
    d->pArray[0].name = (char *)calloc(1, sizeof(char));
    d->pArray[0].id = (char *)calloc(1, sizeof(char));
    d->pArray[0].purchase = 10;

    free(d->pArray);
    free(d);

    // printf("d->pArray[0].name: %s\n", d->pArray[0].name);
    // printf("d->pArray[0].id: %s\n", d->pArray[0].id);
    // printf("d->pArray[0].purchase: %d\n", d->pArray[0].purchase);
}