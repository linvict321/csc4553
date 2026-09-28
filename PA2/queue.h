#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#define MAX 100

typedef struct{
    int key;
    Process process;
}Node;

typedef struct{
    Node items[MAX];
    int size;
}PriorityQueue;



typedef struct{
    int pid;
    int priority;
    char *filename;
    char *params1; //input chars
    int bursttime; //#of times to run it
}Process;

void swap(Node* a, Node*b);
void heapifyUp(PriorityQueue *pg, int index);
void enqueue(PriorityQueue *pq, int value, Process p1);
void heapifyDown(PriorityQueue *pq, int index);
Process dequeue(PriorityQueue *pq);
int peek(PriorityQueue *pq);

#endif