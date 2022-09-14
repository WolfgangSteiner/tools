#ifndef WST_VECTOR_H
#define WST_VECTOR_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define WST_VECTOR_ALLOC_SIZE_DEFAULT 16
#define WST_VECTOR_CALC_ALLOC_SIZE(TYPE, CAPACITY) (sizeof(wst_vector_##TYPE) + CAPACITY * sizeof(TYPE))


#define WST_VECTOR_DEFINE_STRUCT(TYPE) \
    typedef struct\
    {\
        size_t size;\
        size_t capacity;\
        void(*deallocator)(TYPE *);\
        TYPE data[];\
    } wst_vector_##TYPE;\


#define WST_VECTOR_DEFINE_ALLOCATE_WITH_CAPACITY(TYPE) \
    wst_vector_##TYPE* wst_vector_##TYPE##_allocate_with_capacity(size_t capacity)\
    {\
        wst_vector_##TYPE* v = malloc(WST_VECTOR_CALC_ALLOC_SIZE(TYPE, capacity));\
        v->size = 0;\
        v->capacity = capacity;\
        v->deallocator = 0;\
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
        if (index < 0) index += v->size;\
        assert(index >= 0);\
        assert(index < v->size);\
        return v->data[index];\
    }\


#define WST_VECTOR_DEFINE_SET(TYPE)\
    void wst_vector_##TYPE##_set(wst_vector_##TYPE* v, int index, TYPE value)\
    {\
        if (index < 0) index += v->size;\
        assert(index >= 0);\
        assert(index < v->size);\
        v->data[index] = value;\
    }\


#define WST_VECTOR_DEFINE_PUSH_FRONT(TYPE)\
    wst_vector_##TYPE* wst_vector_##TYPE##_pushFront(wst_vector_##TYPE* v, TYPE e)\
    {\
        v = wst_vector_##TYPE##_checkAndGrow(v);\
        memmove(v->data + 1, v->data, v->size * sizeof(TYPE));\
        v->data[0] = e;\
        v->size++;\
        return v;\
    }

#define WST_VECTOR_DEFINE_POP_FRONT(TYPE)\
    TYPE wst_vector_##TYPE##_popFront(wst_vector_##TYPE* v)\
    {\
        TYPE result = v->data[0];\
        if (--v->size)\
        {\
            memmove(v->data, v->data + 1, v->size * sizeof(TYPE));\
        }\
        return result;\
    }

#define WST_VECTOR_DEFINE_FREE(TYPE)\
    void wst_vector_##TYPE##_free(wst_vector_##TYPE* v)\
    {\
        if (v->deallocator)\
        {\
            for (size_t i = 0; i < v->size; ++i)\
            {\
                v->deallocator(&(v->data[i]));\
            }\
        }\
        free(v);\
    }\

#define WST_VECTOR_DEFINE_COPY(TYPE)\
    wst_vector_##TYPE* wst_vector_##TYPE##_copy(wst_vector_##TYPE* v)\
    {\
        wst_vector_##TYPE* v2 = malloc(WST_VECTOR_CALC_ALLOC_SIZE(TYPE, v->size));\
        memcpy(v2, v, WST_VECTOR_CALC_ALLOC_SIZE(TYPE, v->size));\
        return v2;\
    }\


#define WST_VECTOR_DEFINE_EQUAL(TYPE)\
    bool wst_vector_##TYPE##_equal(\
            const wst_vector_##TYPE* v1,\
            const wst_vector_##TYPE* v2)\
    {\
        if (v1->size != v2->size)\
        {\
            return false;\
        }\
        else\
        {\
            for (size_t i = 0; i < v1->size; ++i)\
            {\
                if (v1->data[i] != v2->data[i])\
                {\
                    return false;\
                }\
            }\
            return true;\
        }\
    }\


#define WST_VECTOR_DECLARE(TYPE) \
    WST_VECTOR_DEFINE_STRUCT(TYPE) \
    WST_VECTOR_DEFINE_ALLOCATE_WITH_CAPACITY(TYPE) \
    WST_VECTOR_DEFINE_ALLOCATE(TYPE) \
    WST_VECTOR_DEFINE_REALLOC(TYPE) \
    WST_VECTOR_DEFINE_CHECK_AND_GROW(TYPE) \
    WST_VECTOR_DEFINE_PUSH_BACK(TYPE) \
    WST_VECTOR_DEFINE_PUSH_FRONT(TYPE) \
    WST_VECTOR_DEFINE_POP_BACK(TYPE) \
    WST_VECTOR_DEFINE_POP_FRONT(TYPE) \
    WST_VECTOR_DEFINE_AT(TYPE) \
    WST_VECTOR_DEFINE_SET(TYPE) \
    WST_VECTOR_DEFINE_FREE(TYPE) \
    WST_VECTOR_DEFINE_COPY(TYPE) \
    WST_VECTOR_DEFINE_EQUAL(TYPE) \


#endif
