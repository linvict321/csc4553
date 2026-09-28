#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void swap(Node* a, Node*b){
    Node temp = *a;
    *a = *b;
    *b = temp;
}
void heapifyUp(PriorityQueue *pq, int index){
    if (index
        && pq->items[(index - 1) / 2].key > pq->items[index].key) {
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

    pq->items[pq->size].key = value;
    pq->items[pq->size].process = p1;
    pq->size++;
    heapifyUp(pq, pq->size - 1);
}
void heapifyDown(PriorityQueue *pq, int index){
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pq->size
        && pq->items[left].key < pq->items[smallest].key)
        smallest = left;

    if (right < pq->size
        && pq->items[right].key < pq->items[smallest].key)
        smallest = right;

    if (smallest != index) {
        swap(&pq->items[index], &pq->items[smallest]);
        heapifyDown(pq, smallest);
    }

}
Process dequeue(PriorityQueue *pq){
    if (!pq->size) {
        printf("Priority queue is empty\n");
        Process empty = {0};
        return empty;
    }

    Process item = pq->items[0].process;
    pq->items[0] = pq->items[--pq->size];
    heapifyDown(pq, 0);
    return item;
}
int peek(PriorityQueue *pq){
    if (!pq->size) {
        printf("Priority queue is empty\n");
        return -1;
    }
    return pq->items[0].key;
}
