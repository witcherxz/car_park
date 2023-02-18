#include <stdbool.h>        // Provides boolean data type

typedef struct RandList_t {
	int *list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
} RandList;

void RLinit(int n);

void RLfree();

void RLclear();

void RLadd(int x);

int RLget();

int RLcapacity();

int RLsize();

bool RLisFull();

bool RLisEmpty();