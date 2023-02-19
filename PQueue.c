#include "PQueue.h"
#include <stdio.h>
#include <stdlib.h>

static PQueue pqueue;


/*
 * PQinit:
 * Initialize the priority pqueue
 *  - n            queue capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQinit(int n) {
    pqueue.data = (Car **)malloc(n * sizeof(Car *));
    pqueue.list = (Car **)malloc(n * sizeof(Car *));
    pqueue.capacity = n;
    pqueue.count = 0;
    pqueue.tail = -1;
}

/*
 * PQfree:
 * Free the memory used by the priority pqueue
 *  - n            queue capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQfree() {
    free(pqueue.data);
    free(pqueue.list);
}

/*
 * swap:
 * Helper function to swap two element in the pqueue
 *  - i            index of the first element
 *  - j            index of the second element
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void swap(int i, int j){
    Car* temp1 = pqueue.data[i];
    Car* temp2 = pqueue.list[i];
    
    pqueue.data[i] = pqueue.data[j];
    
    pqueue.data[j] = temp1;
    pqueue.list[j] = temp2;
}

/*
 * PQenqueue:
 * Insert a new element into the priority pqueue
 *  - car            car to be enqueued
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void PQenqueue(Car *car) {
    // Check if the pqueue is full
    if (PQisFull()) {
        printf("Error: Priority pqueue is full\n");
        return;
    }

    pqueue.tail++;
    pqueue.data[pqueue.tail] = car;
    pqueue.list[pqueue.tail] = car;
    for(int i = pqueue.tail; i > 0;i--){
        if(pqueue.data[i]->ltm + pqueue.data[i]->atm > pqueue.data[i - 1]->ltm + pqueue.data[i - 1]->atm) swap(i,i-1);
    }
    pqueue.count++;
}

/*
 * PQserve:
 * Remove the element with the highest priority from the priority pqueue 
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* PQserve() {
    Car *car;

    // Check if the pqueue is empty
    if (PQisEmpty()) {
        printf("Error: Priority pqueue is empty\n");
        return NULL;
    }
    car = pqueue.data[pqueue.tail];
    pqueue.data[pqueue.tail]= NULL;
    pqueue.tail--;
    pqueue.count--;
    return car;
}

/*
 * PQpeek:
 * Return the element with the highest priority without removing it from the pqueue.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* PQpeek() {
    // Check if the pqueue is empty
    if (PQisEmpty()) {
        printf("Error: Priority pqueue is empty\n");
        return NULL;
    }

    return pqueue.data[pqueue.tail];
}

/*
 * PQiterator:
 * Return a list of the pqueue contents and its size.
 *  -sz           return size of the pqueue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car** PQiterator(int *sz){
    int i, j;
    for (i = pqueue.head, j = 0; j < pqueue.count; i = (i + 1) % pqueue.capacity, j++) {
        pqueue.list[j] = pqueue.data[i];
    }
    *sz = pqueue.count;
    return pqueue.list;
}

/*
 * PQsize:
 * Return the number of elements in the priority pqueue 
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int PQsize() {
    return pqueue.count;
}

/*
 * PQcapacity:
 * Return the capacity of the priority pqueue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int PQcapacity() {
    return pqueue.capacity;
}

/*
 * PQisFull:
 * Return true if the priority pqueue is full, false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool PQisFull() {
    return (pqueue.count == pqueue.capacity);
}

/*
 * PQisEmpty:
 * Return true if the priority pqueue is empty, false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool PQisEmpty() {
    return (pqueue.count == 0);
}

