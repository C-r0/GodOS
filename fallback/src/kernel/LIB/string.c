#include "../include/string.h"
#include <stdint.h>

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

int strlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void itoa(uint64_t n, char *str) {
    char temp[24];
    int i = 0;

    if (n == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }

    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0'; 
}

int strncmp(const char *s1, const char *s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int atoi(const char *str) {
    int res = 0;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + str[i] - '0';
        } else {
            break;
        }
    }
    
    return res;
}

void parse_space(char* buf, char** args, int* arg_count) {
    *arg_count = 0;
    char* p = buf;
    int in_quotes = 0;

    while (*p != '\0') {
        while (*p == ' ' && !in_quotes) {
            *p = '\0';
            p++;
        }
        if (*p == '\0') break;

        if (*p == '\"') {
            *p = '\0'; 
            p++;
            if (!in_quotes) {

                args[*arg_count] = p;
                (*arg_count)++;
                in_quotes = 1;
    
                while (*p != '\"' && *p != '\0') p++;
            } else {
                in_quotes = 0;
            }
            continue; 
        } 
        
        // Argumento normal (sem aspas)
        if (!in_quotes) {
            args[*arg_count] = p;
            (*arg_count)++;
            while (*p != ' ' && *p != '\"' && *p != '\0') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
        }
    }
}

uint32_t hex_to_int(char *str) {
    uint32_t val = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (*str) {
        uint8_t byte = *str;
        
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
        else break; 

        val = (val << 4) | (byte & 0xF);
        str++;
    }
    return val;
}

void h_itoa(uint32_t n, char *str) {
    char *p = str;
    char *hex = "0123456789ABCDEF";

    for (int i = 7; i >= 0; i--) {
        p[i] = hex[n & 0xF];
        n >>= 4;
    }
    p[8] = '\0'; 
}
