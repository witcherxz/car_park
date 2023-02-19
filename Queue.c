#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>

static Queue queue;             // queue instance
static pthread_mutex_t q_lock;  // queue lock to implement mutual exclusion

/*
 * Qinit:
 * Initialize the felids of a Queue structure instance.
 *  - n            queue capacity
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void Qinit(int n){
    pthread_mutex_init(&q_lock, NULL);
    queue.data = (Car**) malloc(n * sizeof(Car*));
    queue.list = (Car**) malloc(n * sizeof(Car*));
    queue.capacity = n;
    queue.count = 0;
    queue.head = 0;         // The queue head position
    queue.tail = -1;        // The queue tail position
}

/*
 * Qfree:
 * Free the Queue data and list arrays.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void Qfree(){
    free(queue.data);
    free(queue.list);
}

/*
 * Qclear:
 * Clear the Queue.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void Qclear(){
    queue.count = 0;
    queue.head = 0;
    queue.tail = -1;
}

/*
 * Qenqueue:
 * A FIFO Queue enqueue function. Check precondition Qisfull() = false.
 *  - car            car to be enqueue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
void Qenqueue(Car *car){
    pthread_mutex_lock(&q_lock);
    if (QisFull()) {
        printf("Error: Queue is full.\n");
        pthread_mutex_unlock(&q_lock);
        return;
    }
    queue.tail = (queue.tail + 1) % queue.capacity;
    queue.data[queue.tail] = car;
    queue.count++;
    pthread_mutex_unlock(&q_lock);
}

/*
 * Qserve:
 * A FIFO Queue delete function. Delete and return the car at Queue head. Check precondition QisEmpty = false.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* Qserve(){
    pthread_mutex_lock(&q_lock);
    if (QisEmpty()) {
        printf("Error: Queue is empty.\n");
        pthread_mutex_unlock(&q_lock);
        return NULL;
    }
    Car *car = queue.data[queue.head];
    queue.data[queue.head] = NULL;
    queue.head = (queue.head + 1) % queue.capacity;
    queue.count--;
    pthread_mutex_unlock(&q_lock);
    return car;
}

/*
 * Qpeek:
 * Return the car at the head of the Queue, without deleting it.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car* Qpeek(){
    pthread_mutex_lock(&q_lock);
    Car *car = queue.data[queue.head];
    pthread_mutex_unlock(&q_lock);
    return car;
}

/*
 * Qiterator:
 * Return a list of the queue contents and its size.
 *  -sz           return size of the queue
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
Car** Qiterator(int *sz){
    pthread_mutex_lock(&q_lock);
    int i, j;
    for (i = queue.head, j = 0; j < queue.count; i = (i + 1) % queue.capacity, j++) {
        queue.list[j] = queue.data[i];
    }
    *sz = queue.count;
    pthread_mutex_unlock(&q_lock);
    return queue.list;
}

/*
 * Qcapacity:
 * Return the capacity of the FIFO Queue.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int Qcapacity(){
    return queue.capacity;
}

/*
 * Qsize:
 * Return the capacity of the FIFO Queue.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
int Qsize(){
    return queue.count;
}

/*
 * QisFull:
 * Return true if the FIFO Queue is full. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool QisFull(){
    return queue.count == queue.capacity;
}

/*
 * QisEmpty:
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * 
 * Author Osama Adel Alsahafi
 * Version 1.00 2023/02/10
 */
bool QisEmpty(){
    return queue.count == 0;
}