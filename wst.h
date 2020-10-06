#ifndef WST_H
#define WST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>

#define WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT 16
#define WST_STRING_ARRAY_ALLOC_INCREMENT 16 


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
char* wst_string_new();
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
} wst_string_array;


wst_string_array* wst_string_array_new();
void wst_string_array_delete(wst_string_array* arr);
wst_string_array* wst_string_array_append(wst_string_array* arr, char* s);
wst_string_array* wst_string_array_appendStrings(wst_string_array* arr, char** strings, int count);
wst_string_array* wst_string_array_init(char** str_arr, int count);
wst_string_array* wst_string_array_initWithString(wst_string_array* arr, char* s);
uint wst_string_array_size(wst_string_array* arr);
char* wst_string_array_at(wst_string_array* arr, uint index);
char* wst_string_array_join(wst_string_array* arr, char* joinString);
bool wst_string_array_containsSubString(wst_string_array* array, char* string);
wst_string_array* wst_string_array_grep(wst_string_array* array, char* string);
wst_string_array* wst_string_split(char* s, char c);
char* wst_string_replace(char* s, char* subString, char* replaceString);
wst_string_array* wst_system(char* cmd);
wst_string_array* wst_readlines(char* fileName);

/**
 * Display a menu using the dmenu command.
 * @param prompt  The prompt to display before the menu.
 * @param choices String containing the menu entries, separated by ','.
 * @return String containing the user's choice.
 */
char* wst_dmenu(char* prompt, char* choices);
char* wst_formatPercentBar(float percent, int numSegments, char* symbols);

#endif 
