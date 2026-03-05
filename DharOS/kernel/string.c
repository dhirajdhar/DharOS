#include "string.h"
#include "string.h"

char* strncpy(char* dest, const char* src, unsigned int count) {
    unsigned int i;
    for (i = 0; i < count && src[i]; i++) dest[i] = src[i];
    for (; i < count; i++) dest[i] = '\0';
    return dest;
}

int strncmp(const char* s1, const char* s2, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        if (s1[i] != s2[i]) return (unsigned char)s1[i] - (unsigned char)s2[i];
        if (s1[i] == '\0') break;
    }
    return 0;
}

int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void* memcpy(void* dest, const void* src, unsigned int count) {
    char* d = dest;
    const char* s = src;
    while (count--) *d++ = *s++;
    return dest;
}

void* memset(void* dest, int val, unsigned int count) {
    char* d = dest;
    while (count--) *d++ = (char)val;
    return dest;
}