#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>


char* util_strcpy(char* dest, const char* source, int dest_size);
void util_snprintf(char* dest, int destSize, const char *fmt, ...);
