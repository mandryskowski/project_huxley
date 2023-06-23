#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

static int size = 0;
static const int tp_constant = 240;

// Creates an empty queue
Queue* createQueue()
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Checks if the queue is empty
bool isEmpty(Queue* queue)
{
    return (queue->front == NULL);
}

// Adds element to the end of the queue
void push(Queue* queue, Vec2d data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(queue))
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    size++;
    if (size > tp_constant)
    {
        pop(queue);
    }
}

// Removes element from the beginning of the queue.
Vec2d pop(Queue* queue)
{
    if (isEmpty(queue))
    {
        perror("Queue is empty.\n");
        return (Vec2d){0, 0};
    }

    Node* prev_front = queue->front;
    Vec2d data = prev_front->data;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(prev_front);
    size--;
    return data;
}

void clearQueue(Queue* queue)
{
    while (!isEmpty(queue))
    {
        pop(queue);
    }
}


// Function to free the memory allocated for the queue
void free_queue(Queue* queue)
{
    clearQueue(queue);
    free(queue);
}
