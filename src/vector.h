#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <stdio.h>

struct Vector
{
    void* data;
    int capacity;
    int size;
    int cell;
    char padding[4];
} typedef Vector;

Vector vectorCreate(int, int);
void vectorPush(Vector*, const void*);
bool vectorPop(Vector*, void*);
void* vectorGet(Vector*, int);
void* vectorLast(Vector*);
void vectorClear(Vector*);
void vectorFree(Vector*);
void vectorPrint(Vector* v, void (*printElement)(void*), bool multiline);

#endif