#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"


#define INPUT_STRING_CHUNK_SIZE 10
char* getString(const char* prompt)
{
    printf("%s", prompt);

    char *str = NULL;
    int c;
    int length = 0;

    while ((c = getchar()) != '\n' && c != EOF)
    {
        // Add 1 more chunk of chars to str if we filled the last place
        if (length%INPUT_STRING_CHUNK_SIZE == 0)
            str = (char *)realloc(str, length + INPUT_STRING_CHUNK_SIZE);

        assert(str);

        str[length++] = c;
    }

    str = (char *)realloc(str, length+1); // shrink or expand to fit the whole string + '\0'
    str[length] = '\0';

    return str;
}

int getInt(const char* prompt)
{
    char *str = getString(prompt);
    int input = atoi(str);
    free(str);

    return input;
}
