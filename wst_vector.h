#ifndef WST_VECTOR_H
#define WST_VECTOR_H


#include <assert.h>

#define WST_VECTOR_ALLOC_SIZE_DEFAULT 16


#define WST_VECTOR_DEFINE_STRUCT(VECTOR_TYPE) \
    typedef struct { size_t size; size_t capacity; VECTOR_TYPE data[]; } wst_vector_##VECTOR_TYPE;\


#define WST_VECTOR_CALC_ALLOC_SIZE(TYPE, CAPACITY) (sizeof(wst_vector_##TYPE) + CAPACITY * sizeof(TYPE))


#define WST_VECTOR_DEFINE_ALLOCATE_WITH_CAPACITY(TYPE) \
wst_vector_##TYPE* wst_vector_##TYPE##_allocate_with_capacity(size_t capacity)\
{\
    wst_vector_##TYPE* v = malloc(WST_VECTOR_CALC_ALLOC_SIZE(TYPE, capacity));\
    v->size = 0;\
    v->capacity = capacity;\
    return v;\
}\


#define WST_VECTOR_DEFINE_ALLOCATE(TYPE) \
    wst_vector_##TYPE* wst_vector_##TYPE##_allocate()\
    {\
        return wst_vector_##TYPE##_allocate_with_capacity(WST_VECTOR_ALLOC_SIZE_DEFAULT); \
    }\


#define WST_VECTOR_DEFINE_REALLOC(TYPE) \
    wst_vector_##TYPE* wst_vector_##TYPE##_realloc(wst_vector_##TYPE* v, size_t new_size)\
    {\
        v->capacity = new_size;\
        v = realloc(v, WST_VECTOR_CALC_ALLOC_SIZE(TYPE, new_size));\
    }\


#define WST_VECTOR_DEFINE_CHECK_AND_GROW(TYPE) \
    wst_vector_##TYPE* wst_vector_##TYPE##_checkAndGrow(wst_vector_##TYPE* v)\
    {\
        if (v->size == v->capacity)\
            v = wst_vector_##TYPE##_realloc(v, v->capacity * 2);\
        return v;\
    }\


#define WST_VECTOR_DEFINE_PUSH_BACK(TYPE) \
    wst_vector_##TYPE* wst_vector_##TYPE##_pushBack(wst_vector_##TYPE* v, TYPE e)\
    { \
        v = wst_vector_##TYPE##_checkAndGrow(v);\
        v->data[v->size++] = e;\
        return v;\
    }\


#define WST_VECTOR_DEFINE_POP_BACK(TYPE)\
    TYPE wst_vector_##TYPE##_popBack(wst_vector_##TYPE* v)\
    {\
        assert(v->size);\
        return v->data[--v->size];\
    }\


#define WST_VECTOR_DEFINE_AT(TYPE)\
    TYPE wst_vector_##TYPE##_at(wst_vector_##TYPE* v, int index)\
    {\
        assert(index >= 0);\
        assert(index < v->size);\
        return v->data[index];\
    }\


#define WST_VECTOR_DEFINE_PUSH_FRONT(TYPE)\
    wst_vector_##TYPE* wst_vector_##TYPE##_pushFront(wst_vector_##TYPE* v, TYPE e)\
    {\
        v = wst_vector_##TYPE##_checkAndGrow(v);\
        memmove(v->data + 1, v->data, v->size);\
        v->data[0] = e;\
        v->size++;\
        return v;\
    }


#define WST_VECTOR_DECLARE(TYPE) \
    WST_VECTOR_DEFINE_STRUCT(TYPE) \
    WST_VECTOR_DEFINE_ALLOCATE_WITH_CAPACITY(TYPE) \
    WST_VECTOR_DEFINE_ALLOCATE(TYPE) \
    WST_VECTOR_DEFINE_REALLOC(TYPE) \
    WST_VECTOR_DEFINE_CHECK_AND_GROW(TYPE) \
    WST_VECTOR_DEFINE_PUSH_BACK(TYPE) \
    WST_VECTOR_DEFINE_PUSH_FRONT(TYPE) \
    WST_VECTOR_DEFINE_POP_BACK(TYPE) \
    WST_VECTOR_DEFINE_AT(TYPE) \


#endif
