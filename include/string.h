#pragma once

void *memset(void *s, int c, int n);
int strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
int atoi(char *str);
char* itoa(int num, char *str, int base);
