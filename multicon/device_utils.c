#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "device_utils.h"

void util_get_timestamp(char* timestamp)
{
    time_t ts;
    struct tm* ts_info;

    ts = time(NULL);
    ts_info = localtime(&ts);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", ts_info);
}

int util_contains_char(char* str, char char_to_check)
{
    if (strchr (str, char_to_check) != NULL)
        return 1;
    else
        return 0;    
}

char* util_removechar(char* str, char char_to_replace)
{
    int i,j;
    i = 0;
    while(i<strlen(str))
    {
        if (str[i]==char_to_replace)
        {
            for (j=i; j<strlen(str); j++)
                str[j]=str[j+1];
        }
        else i++;
    }
    return str;
}

void util_strcpy(char* dest, const char* source, int dest_size)
{
    int i ;

    if(source)
    {
        for(i=0; i < dest_size-1 && source[i] !='\0'; i++)
        {
            dest[i] = source[i];
        }
        dest[i] = '\0';
    }
    else
    {
        dest[i] = '\0';
    }
}

void util_snprintf(char* dest, int destSize, const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    vsnprintf(dest, destSize, fmt, args);

    dest[destSize-1] = '\0';
    va_end(args);
}

char** util_strsplit( const char* s, const char* delim ) 
{
	return _strsplit( s, delim, NULL );
}

char** util_strsplit_count( const char* s, const char* delim, size_t* nb ) {
	return _strsplit( s, delim, nb );
}

static char** _strsplit( const char* s, const char* delim, size_t* nb ) 
{
	void* data;
	char* _s = ( char* )s;
	const char** ptrs;
	size_t
		ptrsSize,
		nbWords = 1,
		sLen = strlen( s ),
		delimLen = strlen( delim );

	while ( ( _s = strstr( _s, delim ) ) ) {
		_s += delimLen;
		++nbWords;
	}
	ptrsSize = ( nbWords + 1 ) * sizeof( char* );
	ptrs =
	data = malloc( ptrsSize + sLen + 1 );
	if ( data ) {
		*ptrs =
		_s = strcpy( ( ( char* )data ) + ptrsSize, s );
		if ( nbWords > 1 ) {
			while ( ( _s = strstr( _s, delim ) ) ) {
				*_s = '\0';
				_s += delimLen;
				*++ptrs = _s;
			}
		}
		*++ptrs = NULL;
	}
	if ( nb ) {
		*nb = data ? nbWords : 0;
	}
	return data;
}


/* Esempio di Json inviato dal server ed interpolato dal multicon sulla falsa riga 

{
  "id": 1,
  "method": "call",
  "token": "590bde71578da2fabfe77ba86c00e4e5",
  "params": [
    "uci",
    "get",
    "wireless.@wifi-iface[0].ssid"
  ]
}

{\"id\": 1, \"method\": \"call\", \"token\": \"chiave-di-autenticazione\", \"params\": [ \"uci\", \"get\", \"wireless.@wifi-iface[0].ssid\"] }


*/