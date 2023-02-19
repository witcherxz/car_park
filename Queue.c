#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
static Queue queue;
static pthread_mutex_t q_lock;
/* =============================================================================
 * Initialize the feilds of a Queue structure instance.
 * =============================================================================
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

/* =============================================================================
 * Free the Queue data and list arrays.
 * =============================================================================
 */
void Qfree(){
    free(queue.data);
    free(queue.list);
}

/* =============================================================================
 * Clear the Queue.
 * =============================================================================
 */
void Qclear(){
    queue.count = 0;
    queue.head = 0;
    queue.tail = -1;
}

/* =============================================================================
 * A FIFO Queue enqueue function.
 * Check precondition Qisfull() = false.
 * =============================================================================
 */
void Qenqueue(Car *car){
    pthread_mutex_lock(&q_lock);
    // if (QisFull()) {
    //     printf("Error: Queue is full.\n");
    //     pthread_mutex_unlock(&q_lock);
    //     return;
    // }
    queue.tail = (queue.tail + 1) % queue.capacity;
    queue.data[queue.tail] = car;
    queue.count++;
    pthread_mutex_unlock(&q_lock);
}

/* ===========================================================================
 * A FIFO Queue delete function.
 * Delete and return the car at Queue head.
 * Check precondition QisEmpty = false.
 * ===========================================================================
 */
Car* Qserve(){
    pthread_mutex_lock(&q_lock);
    // if (QisEmpty()) {
    //     printf("Error: Queue is empty.\n");
    //     pthread_mutex_unlock(&q_lock);
    //     return NULL;
    // }
    Car *car = queue.data[queue.head];
    queue.data[queue.head] = NULL;
    queue.head = (queue.head + 1) % queue.capacity;
    queue.count--;
    pthread_mutex_unlock(&q_lock);
    return car;
}

/* ===========================================================================
 * Return the car at the head of the Queue, without deleting it.
 * ===========================================================================
 */
Car* Qpeek(){
    pthread_mutex_lock(&q_lock);
    // if (QisEmpty()) {
    //     printf("Error: Queue is empty.\n");
    //     pthread_mutex_unlock(&q_lock);
    //     return NULL;
    // }
    Car *car = queue.data[queue.head];
    pthread_mutex_unlock(&q_lock);
    return car;
}

/* ===========================================================================
 * Return a list of the queue contents and its size.
 * ===========================================================================
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

/* ===========================================================================
 * Return the capacity of the FIFO Queue.
 * ===========================================================================
 */
int Qcapacity(){
    pthread_mutex_lock(&q_lock);
    int c = queue.capacity;
    pthread_mutex_unlock(&q_lock);
    return c;
}

/* ===========================================================================
 * Return the number of cars in the FIFO Queue.
 * ===========================================================================
 */
int Qsize(){
    pthread_mutex_lock(&q_lock);
    int c = queue.count;
    pthread_mutex_unlock(&q_lock);

    return c;
}

/* ===========================================================================
 * Return true if the FIFO Queue is full. Return false otherwise.
 * ===========================================================================
 */
bool QisFull(){
    pthread_mutex_lock(&q_lock);
    bool f = queue.count == queue.capacity;
    pthread_mutex_unlock(&q_lock);

    return f;
}

/* ===========================================================================
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * ===========================================================================
 */
bool QisEmpty(){
    pthread_mutex_lock(&q_lock);
    bool e = queue.count == 0;
    pthread_mutex_unlock(&q_lock);

    return e;
}
void disp_list(Car ** l){

    for (int i = 0; i < queue.capacity; i++)
    {
        if(l[i]){
            printf("car|");
        }else{
        printf("   |");
        }
    }
    printf("\n");
    
}
void Qdisp(){
    pthread_mutex_lock(&q_lock);
    printf("---------Display---------\n");
    printf("HEAD: %d\n",queue.head);
    printf("TAIL: %d\n",queue.tail);
    printf("COUNT: %d\n",queue.count);
    printf("Data : |");
    disp_list(queue.data);
    printf("List : |");
    disp_list(queue.list);
    printf("-------------------------\n");
    pthread_mutex_unlock(&q_lock);
}