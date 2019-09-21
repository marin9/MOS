#pragma once

#define RED     "\x1B[31m"
#define GREEN	"\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE	"\x1B[34m"
#define MAGENTA	"\x1B[35m"
#define CYAN	"\x1B[36m"
#define GRAY	"\x1B[37m"
#define WHITE   "\x1B[39m"

#define B_RED		"\x1B[1;41m"
#define B_GREEN		"\x1B[1;42m"
#define B_YELLOW	"\x1B[1;43m"
#define B_BLUE		"\x1B[1;44m"
#define B_MAGENTA	"\x1B[1;45m"
#define B_CYAN		"\x1B[1;46m"
#define B_GRAY		"\x1B[1;47m"
#define B_BLACK		"\x1B[1;49m"
#define B_DARKGRAY	"\x1B[1;100m"

int puts(char *s);
int putc(char c);
int putn(int n);
int gets(char *s, int n);
void warn(char *s);
void err(char *s);
void clear();
int getkey();

