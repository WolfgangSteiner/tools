#include "status_format.h"
#include "wst.h"

char* formatStatusField(char* text, char* color) {
    char* formatString = "{\"full_text\":\"%s\",\"color\":\"%s\"}";
    char* result = wst_string_format(formatString, text, color);
    return result;
}
