#include "strdup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}
