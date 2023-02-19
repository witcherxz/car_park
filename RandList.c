#include "RandList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static RandList rl;
//TODO: Clean this code
void RLdisp(int i){
    printf("---------Display RL ---------\n");
    printf("COUNT: %d\n",rl.count);
    printf("LIST: (");
    for (int i = 0; i < rl.count; i++)
    {
        printf(", %d", rl.list[i]);
    }
    printf(")\n");
    printf("List at index (%d): %d\n",i, rl.list[i]);
    printf("-------------------------\n");
}
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

void RLinit(int n){
    rl.list = (int*) malloc(n * sizeof(int));
    rl.capacity = n;
    randomize_list(rl.list, n);
    rl.count = n;
}

void RLfree(){
    free(rl.list);
}

void RLclear(){
    rl.count = 0;
}

void RLadd(int x){
    if(RLisFull()) return;
    rl.list[rl.count] = x;
    rl.count++;
}

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

int RLcapacity(){
    return rl.capacity;
}

int RLsize(){
    return rl.count;
}

bool RLisFull(){
    return rl.count == rl.capacity;
}

bool RLisEmpty(){
    return rl.count == 0;
}

