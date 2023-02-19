#include <stdbool.h>        // Provides boolean data type

typedef struct RandList_t {
	int *list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
} RandList;

/*
 * RLinit:
 * Initialize the felids of a list structure instance.
 *  - n            list capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLinit(int n);

/*
 * RLfree:
 * Free the list data and list arrays.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLfree();

/*
 * RLclear:
 * Clear the list.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLclear();


/*
 * RLadd:
 * Add a value to the end of the list.
 *  - x            number to be added
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLadd(int x);

/*
 * RLget:
 * Get a number at random index and shift the values after it.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLget();

/*
 * RLcapacity:
 * Return the capacity of the list
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLcapacity();

/*
 * RLsize:
 * Return the capacity of the list
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLsize();

/*
 * RLisFull:
 * Return true if the list is full. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool RLisFull();

/*
 * RLisEmpty:
 * Return true if the list is empty. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool RLisEmpty();