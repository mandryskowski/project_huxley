#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void swap(void **this, void **other)
{
    void *helper = *this;
    *this = *other;
    *other = helper;
}

void shuffle(void *array, int num_elements, int element_size) {
    char *ptr = (char *)array;
    int i;
    srand(time(NULL));

    for (i = num_elements - 1; i > 0; --i) {
        int j = rand() % (i + 1);

        // Swap elements
        char *temp = (char *)malloc(element_size);
        char *element_i = ptr + i * element_size;
        char *element_j = ptr + j * element_size;

        memcpy(temp, element_i, element_size);
        memcpy(element_i, element_j, element_size);
        memcpy(element_j, temp, element_size);

        free(temp);
    }
}