#ifndef WST_H
#define WST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#define WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT 16
#define WST_STRING_ARRAY_ALLOC_INCREMENT 16 


typedef float f32;
typedef int32_t i32;
typedef uint32_t u32;

int wst_min(int a, int b);
float wst_minf(float a, float b);
double wst_mind(float a, float b);
int wst_max(int a, int b);
float wst_maxf(float a, float b);
double wst_maxd(float a, float b);

char* wst_string_init(char* s1);
char* wst_string_concatenate(char* s1, char* s2);
char* wst_string_append(char* s1, char* s2);
char* wst_string_appendChar(char* s1, char c);
char* wst_string_appendRange(char* s1, char* s2, uint count);
char* wst_string_join(char* s1, char* s2, char* join_string);
uint wst_string_length(char* s);
char* wst_string_initFromRange(char* s, uint start, uint end);
char* wst_string_new(void);
void wst_string_delete(char* s);
char* wst_string_alloc(uint size);
void wst_string_clear(char* s);
char* wst_string_copy(char* dst, char* src);
bool wst_string_equal(char* s1, char* s2);
void wst_string_rstrip(char* s);
void wst_string_lstrip(char* s);
int wst_string_findCharFromFront(char* s, char c);
int wst_string_findCharFromBack(char* s, char c);
int wst_string_findStringFromFront(char* s, char* subString);
int wst_string_findStringFromBack(char* s, char* subString);
char* wst_string_splitFromFront(char* s, char* splitString);
bool wst_string_startsWith(char* s, char* subString);
bool wst_string_endsWith(char* s, char* subString);
bool wst_string_contains(char* s, char* subString);
char* wst_string_format(char* formatString, ...);
bool wst_is_whitespace(char c);
char* wst_string_getNextToken(char* s);
char* wst_string_getColumn(char* s, int col);

typedef struct
{
  uint count;
  uint alloc_count;
  char* strings[1];
} wst_strarr;


wst_strarr* wst_strarr_new(void);
void wst_strarr_delete(wst_strarr* arr);
wst_strarr* wst_strarr_append(wst_strarr* arr, char* s);
wst_strarr* wst_strarr_appendStrings(wst_strarr* arr, char** strings, int count);
wst_strarr* wst_strarr_init(char** str_arr, int count);
wst_strarr* wst_strarr_initWithString(wst_strarr* arr, char* s);
uint wst_strarr_size(wst_strarr* arr);
char* wst_strarr_at(wst_strarr* arr, uint index);
char* wst_strarr_join(wst_strarr* arr, char* joinString);
bool wst_strarr_containsSubString(wst_strarr* array, char* string);
wst_strarr* wst_strarr_grep(wst_strarr* array, char* string);
wst_strarr* wst_string_split(char* s, char c);
char* wst_string_replace(char* s, char* subString, char* replaceString);
wst_strarr* wst_system(char* cmd);
wst_strarr* wst_readlines(char* fileName);

/**
 * Display a menu using the dmenu command.
 * @param prompt  The prompt to display before the menu.
 * @param choices String containing the menu entries, separated by ','.
 * @return String containing the user's choice.
 */
char* wst_dmenu(char* prompt, char* choices);
char* wst_formatPercentBar(float percent, int numSegments, char* symbols);

#endif 
