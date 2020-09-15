#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "array.h"


// Construction / Destruction

Array array_new(long capacity){
    assert(capacity > 0);
    return (Array){malloc(sizeof(long)*capacity), 0, 0, capacity};
}

void array_destroy(Array a){
    free(a.data);
}


// Primitives

long array_empty(Array a){
    return a.back <= a.front;
}

long array_front(Array a){
    return a.data[a.front];
}

long array_back(Array a){
    return a.data[a.back - 1];
}

void array_popFront(Array* a){
    a->front++;
}

void array_popBack(Array* a){
    a->back--;
}

Array array_save(Array a){
    return (Array){a.data, a.front, a.back, a.capacity};
}


// Iteration

void array_foreach(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popFront(&b)){
        fn(array_front(b));
    }
}

void array_foreachReverse(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popBack(&b)){
        fn(array_back(b));
    }
}

static void _array_printSingleLongHelper(long i){
    printf(", %ld", i);
}

void array_print(Array a){
    printf("Array:{");
    if(!array_empty(a)){
        printf("%ld", array_front(a));
        array_popFront(&a);
    }
    array_foreach(a, _array_printSingleLongHelper);
    printf("}\n");
}


// Capacity

long array_length(Array a){
    return a.back - a.front;
}

void array_reserve(Array* a, long capacity){
    Array new_arr = array_new(capacity);
    for(int i=0; i<a->capacity; i++){
        new_arr.data[i] = a->data[i];
    }
    array_destroy(*a);
    a->data = new_arr.data;
    a->capacity=capacity;
}

void array_insertBack(Array* a, long stuff){
    printf("Array before insert:\n");
    array_print(*a);
    
    const long idx = array_length(*a);
    const int cap_multi = 2;

    if (idx > a->capacity) {
        printf("\n\nTime to double\n\n");
        array_reserve(a, a->capacity * cap_multi);
    }
    a->data[idx] = stuff;
    a->back++;
    printf("Stuff: %ld, index: %ld, capacity: %ld, a[idx]: %ld\n",stuff, idx, a->capacity, a->data[idx]);
    printf("Array after insert:\n");
    array_print(*a);
}


