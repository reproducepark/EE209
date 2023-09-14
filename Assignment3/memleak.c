#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#include "customer_manager.h"

/*--------------------------------------------------------------------*/
float
timedifference_msec(struct timeval* t0, struct timeval* t1)
{
    return (t1->tv_sec - t0->tv_sec) * 1000.0f
		+ (t1->tv_usec - t0->tv_usec) / 1000.0f;
}
/*--------------------------------------------------------------------*/
int
OddNumber(const char *id, const char* name, const int purchase)
{
	const char *p = name + 4;

	if ((atoi(p) % 2) == 1)
		return purchase;

	return 0;
}

void
PerformanceTest(int num) {

	DB_T d;
	int sum, i, res;
	char name[100];
	char id[100];
	struct timeval start, end;
	double elapsed;

	printf("---------------------------------------------------\n" \
		   "  Performance Test\n" \
		   "---------------------------------------------------\n\n");

	d = CreateCustomerDB();
	if (d == NULL) {
		printf("CreateCustomerDB() failed, cannot perform the test\n");
		return;
	}

	/*----------------------- Test 1 ----------------------*/
	printf("[Test 1] Register %d users with RegisterCustomer()\n", num);
	/* start timer */
	gettimeofday(&start, NULL);
	/* run test */
	for (i = 0; i < num; i++) {
		sprintf(name, "name%d", i);
		sprintf(id, "id%d", i);
		if (RegisterCustomer(d, id, name, 10) < 0) {
			printf("RegisterCustomer returns error\n");
			return;
		}
	}
	/* stop timer and calulate elapsed time*/
	gettimeofday(&end, NULL);
	elapsed = timedifference_msec(&start, &end);
	printf("Finished registering %d users\n", num);
	printf("[elapsed time: %f ms]\n\n", elapsed);

	/*----------------------- Test 2 ----------------------*/
	printf("[Test 2] Total sum of purchase of %d users\n"\
		   "         with GetPurchaseByName()\n", num);
	/* start timer */
	gettimeofday(&start, NULL);
	/* run test */
	sum = 0;
	for (i = 0; i < num; i++) {
		sprintf(name, "name%d", i);
		if ((res = GetPurchaseByName(d, name)) > 0)
			sum += res;
	}
	/* stop timer and calulate elapsed time*/
	gettimeofday(&end, NULL);
	elapsed = timedifference_msec(&start, &end);
	printf("Finished calculating the total sum = %d\n", sum);
	printf("[elapsed time: %f ms]\n\n", elapsed);

	/*----------------------- Test 3 ----------------------*/
	printf("[Test 3] Total sum of purchase of %d users\n"\
		   "         with GetPurchaseByID()\n", num);
	/* start timer */
	gettimeofday(&start, NULL);
	/* run test */
	sum = 0;
	for (i = 0; i < num; i++) {
		sprintf(id, "id%d", i);
		if ((res = GetPurchaseByID(d, id)) > 0)
			sum += res;
	}
	/* stop timer and calulate elapsed time*/
	gettimeofday(&end, NULL);
	elapsed = timedifference_msec(&start, &end);
	printf("Finished calculating the total sum = %d\n", sum);
	printf("[elapsed time: %f ms]\n\n", elapsed);

	/*----------------------- Test 4 ----------------------*/
	printf("[Test 4] Total sum of purchase of odd number users\n"\
		   "         with GetSumCustomerPurchase()\n");
	/* start timer */
	gettimeofday(&start, NULL);
	/* run test */
	sum = GetSumCustomerPurchase(d, OddNumber);
	/* stop timer and calulate elapsed time*/
	gettimeofday(&end, NULL);
	elapsed = timedifference_msec(&start, &end);
	printf("Finished calculating the odd number user sum = %d\n", sum);
	printf("[elapsed time: %f ms]\n\n", elapsed);

	/*----------------------- Test 5 ----------------------*/
	printf("[Test 5] Unregister all the %d users\n"\
		   "         with UnregisterCustomerByName()\n", num);
	/* start timer */
	gettimeofday(&start, NULL);
	/* run test */
	for (i = 0; i < num; i++) {
		sprintf(name, "name%d", i);
		assert(UnregisterCustomerByName(d, name) == 0);
	}
	/* stop timer and calulate elapsed time*/
	gettimeofday(&end, NULL);
	elapsed = timedifference_msec(&start, &end);
	printf("Finished unregistering %d users\n", num);
	printf("[elapsed time: %f ms]\n\n", elapsed);


	DestroyCustomerDB(d);
}

int
CorrectnessTest5() {

	DB_T d;
	int result;

	result = 0;
	printf("------------------------------------------------------\n" \
		   "  Correctness Test 5:\n" \
		   "  Register/UnregisterCustomer and GetSumCustomerPurchase\n"\
		   "------------------------------------------------------\n");

	d = CreateCustomerDB();
	if (d == NULL) {
		printf("CreateCustomerDB() failed, cannot perform the test\n");
		return -1;
	}

	result += TestRegisterCustomer(d, "ander2003", "Anderson", 50, 0);
	result += TestRegisterCustomer(d, "Adele", "adele", 100, 0);
	result += TestRegisterCustomer(d, "mike3002", "Mike", 200, 0);
	result += TestRegisterCustomer(d, "adrian", "Adrian", 400, 0);

	result += TestGetSumCustomerPurchase(d, &NameStartsWithA,
										 "NameStartsWithA", 450);
	result += TestGetSumCustomerPurchase(d, &IDStartsWithA,
										 "IDStartsWithA", 100);
	result += TestGetSumCustomerPurchase(d, &PurchaseLargerThan100,
										 "PurchaseLargerThan100", 600);

	result += TestUnregisterCustomerByName(d, "Adrian", 0);
	result += TestRegisterCustomer(d, "Adriano", "Ardriano", 800, 0);

	result += TestGetSumCustomerPurchase(d, &NameStartsWithA,
										 "NameStartsWithA", 850);
	result += TestGetSumCustomerPurchase(d, &IDStartsWithA,
										 "IDStartsWithA", 900);
	result += TestGetSumCustomerPurchase(d, &PurchaseLargerThan100,
										 "PurchaseLargerThan100", 1000);

	DestroyCustomerDB(d);

	printf("\nCorrectness Test 5 %s\n\n",
		   (result >= 0)? "PASSED" : "FAILED!");

	return (result >= 0)? 0 : -1;
}

int main(void){
    PerformanceTest(2000);
    CorrectnessTest5();
    return 0;
}
