/* 
 * WHAT THIS EXAMPLE DOES
 * 
 * We create a pool of 4 threads and then add 40 tasks to the pool(20 task1 
 * functions and 20 task2 functions). task1 and task2 simply print which thread is running them.
 * 
 * As soon as we add the tasks to the pool, the threads will run them. It can happen that 
 * you see a single thread running all the tasks (highly unlikely). It is up the OS to
 * decide which thread will run what. So it is not an error of the thread pool but rather
 * a decision of the OS.
 * 
 * */

#include <stdio.h>
#include <pthread.h>
#include <zconf.h>
#include "thpool.h"


void task1(){
    printf("Start 1\n");
    sleep(1);
	printf("Thread #%u working on task1\n", (int)pthread_self());
}


void task2(){
    printf("Start 2\n");
    sleep(1);
	printf("Thread #%u working on task2\n", (int)pthread_self());
}

void task3(){
    printf("Start 3\n");
    sleep(1);
	printf("Thread #%u working on task3\n", (int)pthread_self());
}

void task4(){
    printf("Start 4\n");
    sleep(1);
	printf("Thread #%u working on task4\n", (int)pthread_self());
}

void task5(){
    printf("Start 5\n");
    sleep(1);
	printf("Thread #%u working on task5\n", (int)pthread_self());
}

void task6(){
    printf("Start 6\n");
    sleep(1);
	printf("Thread #%u working on task6\n", (int)pthread_self());
}

void task7(){
    printf("Start 7\n");
    sleep(1);
	printf("Thread #%u working on task7\n", (int)pthread_self());
}


int main(){
	
	puts("Making threadpool with 2 threads");
	threadpool thpool = ThreadPoolInit(2);


	int i;
	for (i=0; i<2; i++){
        puts("Adding 7 tasks to threadpool");
		ThreadPoolInsertTask(thpool, (void*)task1, NULL);
		ThreadPoolInsertTask(thpool, (void*)task2, NULL);
		ThreadPoolInsertTask(thpool, (void*)task3, NULL);
		ThreadPoolInsertTask(thpool, (void*)task4, NULL);
		ThreadPoolInsertTask(thpool, (void*)task5, NULL);
		ThreadPoolInsertTask(thpool, (void*)task6, NULL);
		ThreadPoolInsertTask(thpool, (void*)task7, NULL);
		sleep(2);
	};

//    puts("Waiting for all jobs to finish");
//    thpool_wait(thpool);

	puts("Killing threadpool");
	ThreadPoolDestroy(thpool);
	
	return 0;
}
