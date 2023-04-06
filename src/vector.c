#include "vector.h"

Vector vectorCreate(int capacity, int cellSize) {
    void* memory = malloc(capacity * cellSize);

    Vector v = {
        .capacity = capacity,
        .cell = cellSize,
        .size = 0,
        .data = memory
    };
    return v;
}

void vectorPush(Vector* v, const void* element) {
    if (v->size == v->capacity) {
        int newCapacity = v->capacity * 2;
        int newMemorySize = newCapacity * v->cell;
        void* newMemory = malloc(newMemorySize);
        memcpy_s(newMemory, newMemorySize, v->data, v->capacity);
        v->capacity = newCapacity;
        free(v->data);
        v->data = newMemory;
    }

    memcpy((char*)(v->data) + (v->size * v->cell), element, v->cell);
    v->size++;
}

bool vectorPop(Vector* v, void* element) {
    if (v->size == 0) {
        return false;
    }

    v->size--;

    if (element != NULL) {
        memcpy(element, (char*)v->data + (v->size * v->cell), v->cell);
    }

    return true;
}

void* vectorGet(Vector* v, int index) {
    if (index < 0 || index >= v->size) {
        return NULL;
    }
    return (char*)v->data + (index * v->cell);
}

void* vectorLast(Vector* v) {
    return (char*)v->data + ((v->size - 1) * v->cell);
}

void vectorClear(Vector* v) {
    v->size = 0;
}

void vectorFree(Vector* v) {
    free(v->data);
    v->size = 0;
    v->capacity = 0;
}

void vectorPrint(Vector* v, void (*printElement)(void*), bool multiline) {
    putchar('[');
    if (multiline) putchar('\n');

    size_t i;
    for (i = 0; i < v->size; i++) {
        if (multiline) putchar('\t');
        printElement(vectorGet(v, i));
        if (i < v->size - 1) {
            putchar(',');
            putchar(' ');
        }
        if (multiline) putchar('\n');
    }

    putchar(']');
    putchar('\n');
}