#include "wst.h"

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

int wst_max(int a, int b)
{
    return a > b ? a : b;
}

float wst_maxf(float a, float b)
{
    return a > b ? a : b;
}

double wst_maxd(float a, float b)
{
    return a > b ? a : b;
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

char* wst_string_appendChar(char* s1, char c)
{
    uint length1 = strlen(s1);
    char* result = realloc(s1, length1 + 1 + 1);
    result[length1] = c;
    result[length1 + 1] = '\0'; 
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

void wst_string_lstrip(char* s)
{
    int idx = 0;

    while (s[idx] == '\n' || s[idx] == '\t' || s[idx] == ' ')
    {
        idx++;
    }

    if (idx > 0)
    {
        memmove(s, &s[idx], strlen(s) - idx);
        s[strlen(s) - idx] = '\0';
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
        return wst_string_init("");
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


bool wst_string_contains(char* s, char* subString)
{
    char* start = strstr(s, subString);
    return start;
}


char* wst_string_format(char* formatString, ...)
{
    va_list argptr;
    va_start(argptr, formatString);
    int length = vsnprintf(0, 0, formatString, argptr);
    va_end(argptr);

    char* result = wst_string_alloc(length);
    va_start(argptr, formatString);
    vsnprintf(result, length + 1, formatString, argptr);
    va_end(argptr);

    return result;    
}

bool wst_is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

char* wst_string_getNextToken(char* s)
{
    char* ptr = s;
    bool isInToken = !wst_is_whitespace(s[0]);

    while (true)
    {
        char c = *ptr;
        
        if (c == '\0') return 0;
        if (!isInToken && !wst_is_whitespace(c)) return ptr;
        if (isInToken && wst_is_whitespace(c)) isInToken = false;
        ptr++;
    }
}

char* wst_string_getColumn(char* s, int col)
{
    char* tokenStart = s;

    for (int i = 0; i < col; ++i)
    {
        tokenStart = wst_string_getNextToken(tokenStart);
        if (tokenStart == 0)
        {
            return wst_string_init("");
        }
    }

    char* tokenEnd = tokenStart; 
    
    while(!wst_is_whitespace(*tokenEnd) && *tokenEnd != '\0')
    {
        tokenEnd++;
    }

    tokenEnd--;

    return wst_string_initFromRange(s, tokenStart - s, tokenEnd - s + 1);
}


wst_string_array* wst_string_array_new()
{
  uint ALLOC_SIZE = sizeof(wst_string_array) + sizeof(char*) * (WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT - 1);
  wst_string_array* arr = malloc(ALLOC_SIZE);
  memset(arr, 0, ALLOC_SIZE);
  arr->alloc_count = WST_STRING_ARRAY_ALLOC_SIZE_DEFAULT;
  return arr;
}

void wst_string_array_delete(wst_string_array* arr)
{
    for (int i = 0; i < arr->count; i++)
    {
        free(arr->strings[i]);
    }

    free(arr);
}


wst_string_array* wst_string_array_append(wst_string_array* arr, char* s)
{
    if (arr->count == arr->alloc_count)
    {
      uint alloc_size = sizeof(wst_string_array) + sizeof(char*) * (arr->alloc_count + WST_STRING_ARRAY_ALLOC_INCREMENT - 1);
      arr = realloc(arr, alloc_size);
      arr->alloc_count += WST_STRING_ARRAY_ALLOC_INCREMENT;
    }

    arr->strings[arr->count] = wst_string_init(s);
    arr->count++;

    return arr;
}



wst_string_array* wst_string_array_appendStrings(wst_string_array* arr, char** strings, int count)
{
    for (int i = 0; i < count; i++)
    {
        arr = wst_string_array_append(arr, strings[i]);
    }

    return arr;
}


wst_string_array* wst_string_array_init(char** str_arr, int count)
{
    wst_string_array* arr = wst_string_array_new();
    for (int i = 0; i < count; ++i)
    {
        arr = wst_string_array_append(arr, str_arr[i]);
    }

    return arr;
}


wst_string_array* wst_string_array_initWithString(wst_string_array* arr, char* s)
{
    return wst_string_array_init(&s, 1);
}


uint wst_string_array_size(wst_string_array* arr)
{
  return arr->count;
}


char* wst_string_array_at(wst_string_array* arr, uint index)
{
  return arr->strings[index];
}


char* wst_string_array_join(wst_string_array* arr, char* joinString)
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
        char* result = wst_string_init(wst_string_array_at(arr, 0));

        for (int i = 1; i < numStrings; i++)
        {
            result = wst_string_append(result, joinString);
            result = wst_string_append(result, wst_string_array_at(arr, i));
        }

        return result;
    }
}

bool wst_string_array_containsSubString(wst_string_array* array, char* string)
{
    for (int i = 0; i < array->count; ++i)
    {
        if (wst_string_contains(array->strings[i], string))
        {
            return true;
        }
    }

    return false;
}

wst_string_array* wst_string_array_grep(wst_string_array* array, char* string)
{
    wst_string_array* result = wst_string_array_new();
    
    for (int i = 0; i < array->count; i++)
    {
        char* s = array->strings[i];
        if (wst_string_contains(s, string))
        {
            result = wst_string_array_append(result, wst_string_init(s));
        }
    }

    return result;
}


wst_string_array* wst_string_split(char* s, char c)
{
  wst_string_array* arr = wst_string_array_new();

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


char* wst_string_replace(char* s, char* subString, char* replaceString)
{
    char* currentStart = s;
    char* currentEnd;
    char* end = s + strlen(s);
    char* result = wst_string_new();

    while (currentEnd = strstr(currentStart, subString))
    {
        result = wst_string_appendRange(result, currentStart, currentEnd - currentStart);
        result = wst_string_append(result, replaceString);
        currentStart = currentEnd + strlen(subString);
    }

    result = wst_string_appendRange(result, currentStart, end - currentStart);
    return result;
}


wst_string_array* wst_system(char* cmd)
{
    FILE* fp;
    
    fp = popen(cmd, "r");
    
    wst_string_array* arr = wst_string_array_new();
    char* buffer = wst_string_alloc(4096);

    while (fgets(buffer, 4096, fp))
    {
        wst_string_rstrip(buffer);
        arr = wst_string_array_append(arr, buffer);
    }

    wst_string_delete(buffer);
    pclose(fp);

    return arr;
}


wst_string_array* wst_readlines(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    
    wst_string_array* arr = wst_string_array_new();
    char* buffer = wst_string_alloc(4096);

    while (fgets(buffer, 4096, fp))
    {
        wst_string_rstrip(buffer);
        arr = wst_string_array_append(arr, buffer);
    }

    wst_string_delete(buffer);
    fclose(fp);

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
        char* choicesFmt = wst_string_replace(choices, ",", "\\n");
        char* cmd = wst_string_init("echo -e \"");
        cmd = wst_string_append(cmd, choicesFmt);
        cmd = wst_string_append(cmd, "\" | dmenu -i ");

        if (wst_string_length(prompt))
        {
            cmd = wst_string_append(cmd, "-p \"");
            cmd = wst_string_append(cmd, prompt);
            cmd = wst_string_append(cmd, "\"");
        }

        wst_string_array* arr = wst_system(cmd);

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

char* wst_formatPercentBar(float percent, int numSegments, char* symbols)
{
    const float numActiveSegments = percent / 100 * numSegments;
    const int numSymbols = strlen(symbols) - 2;
    char* result = wst_string_alloc(numSegments + 2);
    result = wst_string_appendChar(result, symbols[0]);
    for (int i=0; i < numSegments; ++i)
    {
        const float residual = wst_minf(wst_maxf(0.0f, numActiveSegments - i), numSymbols - 1);
        const int idx = wst_min(numSymbols - 1, wst_max(0, (int)(residual * numSymbols)));
        result = wst_string_appendChar(result, symbols[idx + 1]);
    }

    result = wst_string_appendChar(result, symbols[numSymbols + 1]);
}

