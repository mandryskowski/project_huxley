#include "assembleControl.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define DELIMETERS " ,"

// Interval is [start, end)
char *substr(char *string, int start, int end)
{
    assert(end <= strlen(string) && start >= 0 && start < end);
    char *result = malloc((end - start + 1) * sizeof(char));
    strncpy(result, string + start, end - start);
    result[end - start] = '\0';
    printf("%s\n", result);
    return result;
}

// splits instruction into words
char **split(char *instruction)
{
    char **result = malloc(sizeof(char *) * 5);
    char **ptr = result;
    for (char *string = strtok(instruction, DELIMETERS); string != NULL; string = strtok(NULL, DELIMETERS))
    {
        //printf("%s lul\n", string);
        *ptr++ = string;
    }
    result = realloc(result, sizeof(char *) * (ptr - result));
    return result;
}

void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}
