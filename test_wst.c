#include "wst.h"
#include "unity.h"

#include "wst_vector.h"
WST_VECTOR_DECLARE(int)
WST_VECTOR_DECLARE(float)
WST_VECTOR_DECLARE(char)

void setUp(void)
{
}

void tearDown(void)
{
}


void test_wst_string_findStringFromBack()
{
    TEST_ASSERT_EQUAL(-1, wst_string_findStringFromBack("Alpha", "Beta"));
    TEST_ASSERT_EQUAL( 0, wst_string_findStringFromBack("Alpha", "Alpha"));
    TEST_ASSERT_EQUAL( 5, wst_string_findStringFromBack("AlphaAlpha", "Alpha"));
    TEST_ASSERT_EQUAL( 4, wst_string_findStringFromBack("lphaAlpha", "Alpha"));
    TEST_ASSERT_EQUAL( 0, wst_string_findStringFromBack("AlphaAlph", "Alpha"));
}


void test_wst_string_endsWith()
{
    TEST_ASSERT_EQUAL( false, wst_string_endsWith("Alpha", "Beta"));
    TEST_ASSERT_EQUAL( true, wst_string_endsWith("Alpha", "Alpha"));
    TEST_ASSERT_EQUAL( true, wst_string_endsWith("AlphaAlpha", "Alpha"));
    TEST_ASSERT_EQUAL( true, wst_string_endsWith("lphaAlpha", "Alpha"));
    TEST_ASSERT_EQUAL( false, wst_string_endsWith("AlphaAlph", "Alpha"));
}


void test_wst_string_appendRange()
{
    TEST_ASSERT_EQUAL_STRING("AlphaBet", wst_string_appendRange(wst_string_init("Alpha"), "Beta", 3));
    TEST_ASSERT_EQUAL_STRING("Alpha", wst_string_appendRange(wst_string_init("Alpha"), "Beta", 0));       
    TEST_ASSERT_EQUAL_STRING("AlphaBeta", wst_string_appendRange(wst_string_init("Alpha"), "Beta", 10));
}


void test_wst_string_replace()
{
    TEST_ASSERT_EQUAL_STRING("Alpha", wst_string_replace("Alpha", "Beta", "Gamma"));
    TEST_ASSERT_EQUAL_STRING("AlphaGamma", wst_string_replace("AlphaBeta", "Beta", "Gamma"));
    TEST_ASSERT_EQUAL_STRING("GammaAlpha", wst_string_replace("BetaAlpha", "Beta", "Gamma"));
    TEST_ASSERT_EQUAL_STRING("GammaAlphaGamma", wst_string_replace("BetaAlphaBeta", "Beta", "Gamma"));
}


void test_wst_string_lstrip()
{
    char* alpha = wst_string_init("Alpha");
    TEST_ASSERT_EQUAL_STRING("Alpha", alpha);
    wst_string_delete(alpha);
    
    char* beta = wst_string_init(" Beta");
    wst_string_lstrip(beta);
    TEST_ASSERT_EQUAL_STRING("Beta", beta);
    wst_string_delete(beta);

    char* gamma = wst_string_init("    Gamma  ");
    wst_string_lstrip(gamma);
    TEST_ASSERT_EQUAL_STRING("Gamma  ", gamma);
    wst_string_delete(gamma);

    char* delta = wst_string_init("    ");
    wst_string_lstrip(delta);
    TEST_ASSERT_EQUAL_STRING("", delta);
    wst_string_delete(delta);
}


void test_wst_string_getNextToken()
{
    char* s = "  alpha beta   gamma    delta";
    char* token = wst_string_getNextToken(s);
    TEST_ASSERT_EQUAL_STRING("alpha beta   gamma    delta", token);

    token = wst_string_getNextToken(token);
    TEST_ASSERT_EQUAL_STRING("beta   gamma    delta", token);

    token = wst_string_getNextToken(token);
    TEST_ASSERT_EQUAL_STRING("gamma    delta", token);
    
    token = wst_string_getNextToken(token);
    TEST_ASSERT_EQUAL_STRING("delta", token);

    token = wst_string_getNextToken(token);
    TEST_ASSERT_EQUAL(0, token);
}

void test_wst_string_getColumn()
{
    char* s = wst_string_init("alpha beta   gamma    delta");
    TEST_ASSERT_EQUAL_STRING("alpha", wst_string_getColumn(s, 0));
    TEST_ASSERT_EQUAL_STRING("beta", wst_string_getColumn(s, 1));
    TEST_ASSERT_EQUAL_STRING("gamma", wst_string_getColumn(s, 2));
    TEST_ASSERT_EQUAL_STRING("delta", wst_string_getColumn(s, 3));
    TEST_ASSERT_EQUAL_STRING("", wst_string_getColumn(s, 4));
}


void test_wst_strarr_new()
{
    wst_strarr* arr = wst_strarr_new();
    TEST_ASSERT_EQUAL(0, arr->count);
    TEST_ASSERT_EQUAL(WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT, arr->alloc_count);
    wst_strarr_delete(arr);
}


void test_wst_strarr_append()
{
    wst_strarr* arr = wst_strarr_new();

    for (int i = 0; i < WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT + 2 * WST_STRING_ARRAY_ALLOC_INCREMENT + 1; i++)
    {
        arr = wst_strarr_append(arr, "Alpha");
        TEST_ASSERT_EQUAL(i + 1, wst_strarr_size(arr));
        TEST_ASSERT_EQUAL_STRING("Alpha", wst_strarr_at(arr, i));
    }

    wst_strarr_delete(arr);
}


size_t calc_alloc_size(size_t size)
{
    uint mask = WST_VECTOR_ALLOC_SIZE_DEFAULT - 1;
    
    while (((size - 1) & mask) < size - 1)
    {
        mask = (mask << 1) | 1 ;
    }

    return mask + 1;
}

char test_wst_vector_char_deallocator_buffer[256] = {};

void test_wst_vector_char_deallocator(char* c)
{ 
    size_t idx = strlen(test_wst_vector_char_deallocator_buffer);
    test_wst_vector_char_deallocator_buffer[idx] = *c;
}


void test_wst_vector_char()
{
    wst_vector_char* v = wst_vector_char_allocate();
    const char* test_string = "!!!Hello World!!!";
    for (int i = 3; i < strlen(test_string); ++i)
    {
        v = wst_vector_char_pushBack(v, test_string[i]);
    }

    for (int i = 0; i < 3; ++i)
    {
        v = wst_vector_char_pushFront(v, test_string[i]);
    }

    v = wst_vector_char_pushBack(v, 0);
    TEST_ASSERT_EQUAL_STRING(test_string, v->data);
    TEST_ASSERT_EQUAL(calc_alloc_size(v->size), v->capacity);
    v->deallocator = test_wst_vector_char_deallocator;

    wst_vector_char* v2 = wst_vector_char_copy(v);
    TEST_ASSERT_EQUAL_STRING(test_string, v2->data);

    wst_vector_char_free(v2);
    TEST_ASSERT_EQUAL_STRING(test_string, test_wst_vector_char_deallocator_buffer);

    TEST_ASSERT_EQUAL(test_string[0], wst_vector_char_popFront(v));
    TEST_ASSERT_EQUAL_STRING(&test_string[1], v->data);
    TEST_ASSERT_EQUAL(strlen(test_string) - 1, v->size - 1);
}


void test_wst_vector_int()
{
    wst_vector_int* v = wst_vector_int_allocate();
    for (int i = 0; i < 1000; i++)
    {
        v = wst_vector_int_pushBack(v, i);
        TEST_ASSERT_EQUAL(i, v->data[i]);
        TEST_ASSERT_EQUAL(calc_alloc_size(v->size), v->capacity);
    }
}


void test_wst_vector_float()
{
    wst_vector_float* v = wst_vector_float_allocate();
    for (int i = 0; i < 1000; i++)
    {
        v = wst_vector_float_pushBack(v, i);
        TEST_ASSERT_EQUAL(i, v->data[i]);
        TEST_ASSERT_EQUAL(calc_alloc_size(v->size), v->capacity);
    }

    TEST_ASSERT_EQUAL(999, wst_vector_float_at(v, -1));
    TEST_ASSERT_EQUAL(0, wst_vector_float_at(v, 0));

    wst_vector_float* v2 = wst_vector_float_copy(v);
    TEST_ASSERT(wst_vector_float_equal(v, v2));
    wst_vector_float_set(v2, -1, 42);
    TEST_ASSERT_FALSE(wst_vector_float_equal(v, v2));
    wst_vector_float_popBack(v2);
    TEST_ASSERT_FALSE(wst_vector_float_equal(v, v2));
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_wst_string_findStringFromBack);
    RUN_TEST(test_wst_string_endsWith);
    RUN_TEST(test_wst_string_appendRange);
    RUN_TEST(test_wst_string_replace);
    RUN_TEST(test_wst_string_lstrip);
    RUN_TEST(test_wst_string_getNextToken);
    RUN_TEST(test_wst_string_getColumn);
    RUN_TEST(test_wst_strarr_new);
    RUN_TEST(test_wst_strarr_append);
    RUN_TEST(test_wst_vector_char);
    RUN_TEST(test_wst_vector_int);
    RUN_TEST(test_wst_vector_float);
    return UNITY_END();
}    
