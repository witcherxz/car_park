#include <pthread.h>		// Provides thread-safe mutex lock
#include <stdbool.h>        // Provides boolean data type
#include "Car.h"			// Provides the required Car type structure

typedef struct PQueue_t {
    Car **data;       		// Array to hold car queue
	Car **list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
    int tail;             	// The queue tail position
    int head;            	// The queue head position
} PQueue;

/*
 * PQinit:
 * Initialize the priority pqueue
 *  - n            queue capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQinit(int n);

/*
 * PQfree:
 * Free the memory used by the priority pqueue
 *  - n            queue capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQfree();

void PQclear();

/*
 * PQenqueue:
 * Insert a new element into the priority pqueue
 *  - car            car to be enqueued
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQenqueue(Car *car);

/*
 * PQserve:
 * Remove the element with the highest priority from the priority pqueue 
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* PQserve();

/*
 * PQpeek:
 * Return the element with the highest priority without removing it from the pqueue.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* PQpeek();

/*
 * PQiterator:
 * Return a list of the pqueue contents and its size.
 *  -sz           return size of the pqueue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car** PQiterator(int *sz);

/*
 * PQcapacity:
 * Return the capacity of the priority pqueue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int PQcapacity();

/*
 * PQsize:
 * Return the number of elements in the priority pqueue 
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int PQsize();

/*
 * PQisFull:
 * Return true if the priority pqueue is full, false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool PQisFull();

/*
 * PQisEmpty:
 * Return true if the priority pqueue is empty, false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool PQisEmpty();
