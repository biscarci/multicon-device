#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include "system_logger.h"
#include "device_utils.h"

static const char* log_level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};


void print_log_console(int log_level, char* timestamp, char* module, char* message_string)
{
    switch(log_level)
    {
        case LOGGER_ERROR:
            printf("%s %7s " ANSI_COLOR_RED     "%6s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
        break;

        case LOGGER_WARN:
            printf("%s %7s " ANSI_COLOR_YELLOW  "%6s "  ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
        break;

        case LOGGER_INFO:
            printf("%s %7s " ANSI_COLOR_GREEN   "%6s "  ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
        break;

        case LOGGER_DEBUG:
            printf("%s %7s " ANSI_COLOR_BLUE    "%6s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
        break;

        case LOGGER_FATAL:
            printf("%s %7s " ANSI_COLOR_MAGENTA "%6s " ANSI_COLOR_RESET "%s\n", timestamp, module, log_level_strings[log_level], message_string);
            exit(EXIT_FAILURE);
        break;
    }
}

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
        
        // Stampo in console il log
        print_log_console(log_level, timestamp, module, message_string);

        FILE *f;
        static char filename[100];
        char filename_timestamp[50]; 
        
        long logfile_size;
        static int new_file_created = 0;
        
        // Costruisco la stringa del timestamp per il file di log
        strftime(filename_timestamp, 26, "%Y_%m_%d", ts_info);

        if(!new_file_created)    util_snprintf(filename, sizeof(filename), "%s_%s.csv", "multicon_log", filename_timestamp);
        else                     util_snprintf(filename, sizeof(filename), "%s_%s_%d.csv", "multicon_log", filename_timestamp, new_file_created);
        
        

        f = fopen(filename, "a"); // (append) option will allow appending which is useful in a log file

        fseek(f, 0, SEEK_END);
        logfile_size = ftell(f);
        if(logfile_size >= 2000000)
        {
            // Se il file è più grande di 2mb, ne creo uno nuovo
            print_log_console(LOGGER_WARN, timestamp, "LOGGER", "Maximum file size of 2mb is reached for the current file log");
            
            // Resetto il nome del file
            memset(filename,0,sizeof(filename));
            fclose(f);
            new_file_created++;
            util_snprintf(filename, sizeof(filename), "%s_%s_%d.csv", "multicon_log", filename_timestamp, new_file_created);
            
            // Riapro il file con un nuovo nome
            f = fopen(filename, "a");
        }
                            
        
        
        if (f == NULL) 
        { 
            print_log_console(LOGGER_FATAL, timestamp, "LOGGER", "Unable to store log");
        }
        else
        {
            fprintf(f, "%s; %s; %s; %s\n", timestamp, module, log_level_strings[log_level], message_string);
            fclose(f);
        } 
    }
    
}

void logger_init()
{
    /* char cmd[100];
    // Costruisco comando per la cancellazione dei log vecchi di una settimana
    util_snprintf(cmd, sizeof(cmd), "find ./ -iname \"multicon_log*\" -mtime +7 -delete");
    int ret = system(cmd);
    
    if(ret!=0)
    {
        system_logger(LOGGER_WARN,"LOGGER", "Deleted file one week old (ret=%d)", ret);
    } */
}