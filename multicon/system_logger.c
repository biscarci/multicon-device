#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#include "system_logger.h"
#include "device_utils.h"

static const char* log_level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};


void system_logger(int log_level, char* module, const char* fmt, ...)
{
    char message_string[MAX_MESSAGE_LENGTH];
    if(log_level >= min_log_level)
    {
        va_list args;
        va_start (args, fmt);
        char timestamp[50]; 
        time_t ts;
        struct tm* ts_info;

        // Stampa la stringa correttamente
        message_string[sizeof(message_string)-1] = '\0';
        vsnprintf(message_string, sizeof(message_string)-1, fmt, args );
        va_end(args);
        
        // Costruisco la stringa del timestamp
        ts = time(NULL);
        ts_info = localtime(&ts);

        strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", ts_info);

        switch(log_level)
        {
            case LOGGER_ERROR:
                printf("%s %7s " ANSI_COLOR_RED     "%5s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
            break;

            case LOGGER_WARN:
                printf("%s %7s " ANSI_COLOR_YELLOW  "%5s "  ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
            break;

            case LOGGER_INFO:
                printf("%s %7s " ANSI_COLOR_GREEN   "%5s "  ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
            break;

            case LOGGER_DEBUG:
                printf("%s %7s " ANSI_COLOR_BLUE    "%5s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
            break;

            case LOGGER_FATAL:
                printf("%s %7s " ANSI_COLOR_MAGENTA "%5s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
                exit(EXIT_FAILURE);
            break;
        }

        FILE *f;

        f = fopen("multicon_log.csv", "a"); // (append) option will allow appending which is useful in a log file
        if (f == NULL) 
        { 
            printf("%s %7s " ANSI_COLOR_RED     "%5s " ANSI_COLOR_RESET "%s\n",timestamp, "LOGGER", "FATAL", "Unable to store log" );
        }
        else
        {
            fprintf(f, "%s, %s, %s, %s\n", timestamp, module, log_level_strings[log_level], message_string);
            fclose(f);
        } 
        
        
    }
    
}
