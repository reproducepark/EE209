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
