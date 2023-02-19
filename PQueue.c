#include "PQueue.h"
#include <stdio.h>
#include <stdlib.h>

static PQueue pqueue;

void PQdisp(){
    printf("--------------------------------------------------------------------------\n");
    printf("CARS: {");
    for (int i = pqueue.tail; i >= 0; i--){
    printf(", car: {cid: %d, vid: %d, cno: %d}", pqueue.data[i]->cid, pqueue.data[i]->vid,  pqueue.data[i]->sno);
    }
    printf(" }\n");
    printf("--------------------------------------------------------------------------\n");
    // printf("List access: %ld\n",pqueue.list[pqueue.tail]->ltm);
}


/* Initialize the priority pqueue */
void PQinit(int n) {
    pqueue.data = (Car **)malloc(n * sizeof(Car *));
    pqueue.list = (Car **)malloc(n * sizeof(Car *));
    pqueue.capacity = n;
    pqueue.count = 0;
    pqueue.tail = -1;
}

/* Free the memory used by the priority pqueue */
void PQfree() {
    free(pqueue.data);
    free(pqueue.list);
}


void swap(int i, int j){
    Car* temp1 = pqueue.data[i];
    Car* temp2 = pqueue.list[i];
    
    pqueue.data[i] = pqueue.data[j];
    
    pqueue.data[j] = temp1;
    pqueue.list[j] = temp2;
}

/* Insert a new element into the priority pqueue */
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

/* Remove the element with the highest priority from the priority pqueue */
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


/* Return the element with the highest priority without removing it from the pqueue */
Car* PQpeek() {
    // Check if the pqueue is empty
    if (PQisEmpty()) {
        printf("Error: Priority pqueue is empty\n");
        return NULL;
    }

    return pqueue.data[pqueue.tail];
}
// FIX: iterator
Car** PQiterator(int *sz){
    int i, j;
    for (i = pqueue.head, j = 0; j < pqueue.count; i = (i + 1) % pqueue.capacity, j++) {
        pqueue.list[j] = pqueue.data[i];
    }
    *sz = pqueue.count;
    return pqueue.list;
}

/* Return the number of elements in the priority pqueue */
int PQsize() {
    return pqueue.count;
}

/* Return the capacity of the priority pqueue */
int PQcapacity() {
    return pqueue.capacity;
}

/* Return 1 if the priority pqueue is full, 0 otherwise */
bool PQisFull() {
    return (pqueue.count == pqueue.capacity);
}

/* Return 1 if the priority pqueue is empty, 0 otherwise */
bool PQisEmpty() {
    return (pqueue.count == 0);
}

