#ifndef STRING_H
#define STRING_H

#include <stdint.h> 

int strcmp(const char *a, const char *b);
int strlen(const char *str);
void itoa(uint64_t n, char *str);
int strncmp(const char *s1, const char *s2, int n);
int atoi(const char *str);
void parse_space(char* buf, char** args, int* arg_count);
uint32_t hex_to_int(char *str);
void h_itoa(uint32_t n, char *str);

#endif