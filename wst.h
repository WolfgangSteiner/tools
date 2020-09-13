#ifndef WST_H
#define WST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


#define WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT 16
#define WST_STRING_ARRAY_ALLOC_INCREMENT 16 


int wst_min(int a, int b)
{
    return a < b ? a : b;
}

float wst_minf(float a, float b)
{
    return a < b ? a : b;
}

double wst_mind(float a, float b)
{
    return a < b ? a : b;
}


char* wst_string_concatenate(char* s1, char* s2)
{
    uint length1 = strlen(s1);
    uint length2 = strlen(s2);
    uint length = length1 + length2;
    char* result = (char*) malloc(sizeof(char) * (length + 1));
    strcpy(result, s1);
    strcpy(result + length1, s2);
    return result;
}


char* wst_string_append(char* s1, char* s2)
{
    uint length1 = strlen(s1);
    uint length2 = strlen(s2);
    char* result = realloc(s1, length1 + length2 + 1);
    strcpy(result + length1, s2);
    return result;
}


char* wst_string_appendRange(char* s1, char* s2, uint count)
{
    uint length1 = strlen(s1);
    uint length2 = wst_min(strlen(s2), count);
    char* result = realloc(s1, length1 + length2 + 1);
    strncpy(result + length1, s2, count);
    return result;
}


char* wst_string_join(char* s1, char* s2, char* join_string)
{
    uint len1 = strlen(s1);
    uint len2 = strlen(s2);
    uint len3 = strlen(join_string);
    uint len = len1 + len2 + len3;
    char* s = (char*) malloc(sizeof(char) * (len + 1));
    strcpy(s, s1);
    strcpy(s + len1, join_string);
    strcpy(s + len1 + len3, s2);
    return s;
}


char* wst_string_init(char* s1)
{
    char* result = malloc(strlen(s1) + 1);
    strcpy(result, s1);
    return result;
}


uint wst_string_length(char* s)
{
    return strlen(s);
}


char* wst_string_initFromRange(char* s, uint start, uint end)
{
    assert(end <= wst_string_length(s));
    char* result = malloc(end - start + 1);
    strncpy(result, s + start, end - start);
    result[end - start] = 0;
    return result;
}


char* wst_string_new()
{
    char* result = malloc(32);
    memset(result, 0, 32);
    return result;
}


void wst_string_delete(char* s)
{
    free(s);
}


char* wst_string_alloc(uint size)
{
  char* s = malloc(size + 1);
  memset(s, 0, size + 1);
  return s;
}


void wst_string_clear(char* s)
{
    memset(s, 0, strlen(s));
}


char* wst_string_copy(char* dst, char* src)
{
    wst_string_clear(dst);
    return wst_string_append(dst, src);
}


bool wst_string_equal(char* s1, char* s2)
{
   return 0 == strcmp(s1, s2);
}


void wst_string_rstrip(char* s)
{
    for (int idx = strlen(s) - 1; idx >= 0; idx--)
    {
        char c = s[idx];

        if (c == '\n' || c == '\t' || c == ' ')
        {
            s[idx] = 0;
        }
        else
        {
            break;
        }
    }
}


int wst_string_findCharFromFront(char* s, char c)
{
    char* charPtr = strchr(s, c);
    if (charPtr == 0)
    {
        return -1;
    }
    else
    {
        return charPtr - s;
    }
}    


int wst_string_findCharFromBack(char* s, char c)
{
    char* charPtr = strrchr(s, c);

    if (charPtr == 0)
    {
        return -1;
    }
    else
    {
        return charPtr - s;
    }
}


int wst_string_findStringFromFront(char* s, char* subString)
{
    char* charPtr = strstr(s, subString);
    if (charPtr == 0)
    {
        return -1;
    }
    else
    {
        return charPtr - s;
    }
}    


int wst_string_findStringFromBack(char* s, char* subString)
{
    char* currentPtr = strstr(s, subString);
    char* nextPtr;

    while (currentPtr && (nextPtr = strstr(currentPtr + strlen(subString), subString)))
    {
        currentPtr = nextPtr;
    }

    if (currentPtr)
    {
        return currentPtr - s;
    }
    else
    {
        return -1;
    }
}


char* wst_string_splitFromFront(char* s, char* splitString)
{
    uint pos = wst_string_findStringFromFront(s, splitString);
    if (pos == -1)
    {
        return wst_string_init(s);
    }
    else if (pos == 0)
    {
        return "";
    }
    else
    {
        return wst_string_initFromRange(s, 0, pos);
    }
}


bool wst_string_startsWith(char* s, char* subString)
{
    if (wst_string_length(subString) > wst_string_length(s))
    {
        return false;
    }
    else
    {
        int pos = wst_string_findStringFromBack(s, subString);
        return pos == 0;
    }
}


bool wst_string_endsWith(char* s, char* subString)
{
    if (wst_string_length(subString) > wst_string_length(s))
    {
        return false;
    }
    else
    {
        int pos = wst_string_findStringFromBack(s, subString);
        return pos == wst_string_length(s) - wst_string_length(subString);
    }
}


typedef struct
{
  uint count;
  uint alloc_count;
  char* strings[1];
} wst_string_array_t;


typedef char* wst_string;
typedef wst_string_array_t* wst_string_array;


wst_string_array wst_string_array_new()
{
  uint ALLOC_SIZE = sizeof(wst_string_array_t) + sizeof(char*) * (WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT - 1);
  wst_string_array arr = malloc(ALLOC_SIZE);
  memset(arr, 0, ALLOC_SIZE);
  arr->alloc_count = WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT;
  return arr;
}

void wst_string_array_delete(wst_string_array arr)
{
    for (int i = 0; i < arr->count; i++)
    {
        free(arr->strings[i]);
    }

    free(arr);
}


wst_string_array wst_string_array_append(wst_string_array arr, char* s)
{
    if (arr->count == arr->alloc_count)
    {
      uint alloc_size = sizeof(wst_string_array_t) + sizeof(char*) * (arr->alloc_count + WST_STRING_ARRAY_ALLOC_INCREMENT - 1);
      arr = realloc(arr, alloc_size);
      arr->alloc_count += WST_STRING_ARRAY_ALLOC_INCREMENT;
    }

    arr->strings[arr->count] = wst_string_init(s);
    arr->count++;

    return arr;
}



wst_string_array wst_string_array_appendStrings(wst_string_array arr, char** strings, int count)
{
    for (int i = 0; i < count; i++)
    {
        arr = wst_string_array_append(arr, strings[i]);
    }

    return arr;
}


wst_string_array wst_string_array_init(char** str_arr, int count)
{
    wst_string_array arr = wst_string_array_new();
    for (int i = 0; i < count; ++i)
    {
        arr = wst_string_array_append(arr, str_arr[i]);
    }

    return arr;
}


wst_string_array wst_string_array_initWithString(wst_string_array arr, char* s)
{
    return wst_string_array_init(&s, 1);
}


uint wst_string_array_size(wst_string_array arr)
{
  return arr->count;
}


char* wst_string_array_at(wst_string_array arr, uint index)
{
  return arr->strings[index];
}


wst_string wst_string_array_join(wst_string_array arr, char* joinString)
{
    uint numStrings = wst_string_array_size(arr);

    if (numStrings == 0)
    {
        return wst_string_new();
    }
    else if (numStrings == 1)
    {
        return wst_string_init(wst_string_array_at(arr, 0));
    }
    else
    {
        wst_string result = wst_string_init(wst_string_array_at(arr, 0));

        for (int i = 1; i < numStrings; i++)
        {
            result = wst_string_append(result, joinString);
            result = wst_string_append(result, wst_string_array_at(arr, i));
        }

        return result;
    }
}


wst_string_array wst_string_split(char* s, char c)
{
  wst_string_array arr = wst_string_array_new();

  char currentChar = s[0];
  uint currentIndex = 0;
  uint currentStartIndex = 0;

  while(currentChar != 0u)
  {
    if (currentChar == c)
    {
      arr = wst_string_array_append(arr, wst_string_initFromRange(s, currentStartIndex, currentIndex));
      currentStartIndex = currentIndex + 1;
    }
    currentIndex++;
    currentChar = s[currentIndex];
  }

//  if (currentStartIndex < currentIndex)
  {
    arr = wst_string_array_append(arr, wst_string_initFromRange(s, currentStartIndex, currentIndex));
  }

  return arr;
}


wst_string wst_string_replace(char* s, char* subString, char* replaceString)
{
    char* currentStart = s;
    char* currentEnd;
    char* end = s + strlen(s);
    wst_string result = wst_string_new();

    while (currentEnd = strstr(currentStart, subString))
    {
        result = wst_string_appendRange(result, currentStart, currentEnd - currentStart);
        result = wst_string_append(result, replaceString);
        currentStart = currentEnd + strlen(subString);
    }

    result = wst_string_appendRange(result, currentStart, end - currentStart);
    return result;
}


wst_string_array wst_system(char* cmd)
{
    FILE* fp;
    
    fp = popen(cmd, "r");
    
    wst_string_array arr = wst_string_array_new();
    char* buffer = wst_string_alloc(4096);

    while (fgets(buffer, 4096, fp))
    {
        wst_string_rstrip(buffer);
        arr = wst_string_array_append(arr, buffer);
    }

    return arr;
}


/**
 * Display a menu using the dmenu command.
 * @param prompt  The prompt to display before the menu.
 * @param choices String containing the menu entries, separated by ','.
 * @return String containing the user's choice.
 */
char* wst_dmenu(char* prompt, char* choices)
{
    char* result = wst_string_new();

    if (wst_string_length(choices) > 0)
    {
        wst_string choicesFmt = wst_string_replace(choices, ",", "\\n");
        wst_string cmd = wst_string_init("echo -e \"");
        cmd = wst_string_append(cmd, choicesFmt);
        cmd = wst_string_append(cmd, "\" | dmenu -i ");

        if (wst_string_length(prompt))
        {
            cmd = wst_string_append(cmd, "-p \"");
            cmd = wst_string_append(cmd, prompt);
            cmd = wst_string_append(cmd, "\"");
        }

        wst_string_array arr = wst_system(cmd);

        if (wst_string_array_size(arr))
        {
            result = wst_string_init(wst_string_array_at(arr, 0));
        }
        
        wst_string_delete(choicesFmt);
        wst_string_delete(cmd);
        wst_string_array_delete(arr);
    }

    return result;
} 

#endif 
