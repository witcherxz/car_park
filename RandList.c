#include "RandList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static RandList rl;

/*
 * randomize_list:
 * Helper function initialize a list with values between [ 0 - size-1] and shuffle it.
 * 
 *  -list           the list to initialized
 *  -size           size of the list
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void randomize_list(int* list, int size){
    for (int i = 0; i < size; i++) list[i] = i;
    // shuffle list
    srand(time(NULL));
    for (int i = 0; i < size; i++)
    {
        int si = rand() % size; 
        int temp = list[i];
        list[i] = list[si];
        list[si] = temp;
    }
}

/*
 * RLinit:
 * Initialize the felids of a list structure instance.
 *  - n            list capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLinit(int n){
    rl.list = (int*) malloc(n * sizeof(int));
    rl.capacity = n;
    randomize_list(rl.list, n);
    rl.count = n;
}

/*
 * RLfree:
 * Free the list data and list arrays.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLfree(){
    free(rl.list);
}

/*
 * RLclear:
 * Clear the list.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLclear(){
    rl.count = 0;
}

/*
 * RLadd:
 * Add a value to the end of the list.
 *  - x            number to be added
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void RLadd(int x){
    if(RLisFull()) return;
    rl.list[rl.count] = x;
    rl.count++;
}

/*
 * RLget:
 * Get a number at random index and shift the values after it.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLget(){
    srand(time(NULL));
    int ri = rand() % rl.count;
    int rv = rl.list[ri];
    for (int i = ri; i < rl.count - 1; i++)
    {
        rl.list[i] = rl.list[i+1];
    }
    rl.count--;
    return rv;
}

/*
 * RLcapacity:
 * Return the capacity of the list
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLcapacity(){
    return rl.capacity;
}

/*
 * RLsize:
 * Return the capacity of the list
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int RLsize(){
    return rl.count;
}

/*
 * RLisFull:
 * Return true if the list is full. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool RLisFull(){
    return rl.count == rl.capacity;
}

/*
 * RLisEmpty:
 * Return true if the list is empty. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool RLisEmpty(){
    return rl.count == 0;
}

