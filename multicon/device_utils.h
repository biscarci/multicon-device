


int util_contains_char(char* str, char char_to_check);
char* util_strcpy(char* dest, const char* source, int dest_size);
char* util_removechar(char* str, char char_to_replace);
void util_snprintf(char* dest, int destSize, const char *fmt, ...);

char** util_strsplit( const char* s, const char* delim );
char** util_strsplit_count( const char* s, const char* delim, size_t* nb );
static char** _strsplit( const char* s, const char* delim, size_t* nb );