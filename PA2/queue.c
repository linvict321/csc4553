#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#define MAX 100

void swap(int* a, int*b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
void heapifyUp(PriorityQueue *pq, int index){
    if (index
        && pq->items[(index - 1) / 2] > pq->items[index]) {
        swap(&pq->items[(index - 1) / 2],
             &pq->items[index]);
        heapifyUp(pq, (index - 1) / 2);
    }
}
void enqueue(PriorityQueue *pq, int value, Process p1){
    if (pq->size == MAX) {
        printf("Priority queue is full\n");
        return;
    }

    pq->items[pq->size++] = p1;
    heapifyUp(pq, pq->size - 1);
}
int heapifyDown(PriorityQueue *pq, int index){
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pq->size
        && pq->items[left] < pq->items[smallest])
        smallest = left;

    if (right < pq->size
        && pq->items[right] < pq->items[smallest])
        smallest = right;

    if (smallest != index) {
        swap(&pq->items[index], &pq->items[smallest]);
        heapifyDown(pq, smallest);
    }
}
int dequeue(PriorityQueue *pq){
    if (!pq->size) {
        printf("Priority queue is empty\n");
        return -1;
    }

    int item = pq->items[0];
    pq->items[0] = pq->items[--pq->size];
    heapifyDown(pq, 0);
    return item;
}
int peek(PriorityQueue *pq){
    if (!pq->size) {
        printf("Priority queue is empty\n");
        return -1;
    }
    return pq->items[0];
}