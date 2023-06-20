#ifndef ARMV8_25_QUEUE_H
#define ARMV8_25_QUEUE_H

#include "game_math.h"

typedef struct Node {
    Vec2d data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

Queue* createQueue();
void push(Queue* queue, Vec2d data);
Vec2d pop(Queue* queue);
bool isEmpty(Queue *queue);
void clearQueue(Queue* queue);
void freeQueue(Queue* queue);


#endif //ARMV8_25_QUEUE_H
