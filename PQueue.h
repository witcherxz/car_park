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

void swap(int i, int j);

void PQinit(int n);

void PQfree();

void PQclear();

void PQenqueue(Car *car);

Car* PQserve();

Car* PQpeek();

Car** PQiterator(int *sz);

int PQcapacity();

int PQsize();

void PQdisp();

bool PQisFull();

bool PQisEmpty();
