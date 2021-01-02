#include "device_settings.h"
#include "device_utils.h"
#include "system_logger.h"



int device_settings_execute_commands(char* commands, int command_method)
{
    char *shell_output = NULL;
    int shell_result = -1;

    if( command_method == METHOD_UCI_GET)
    {
        shell_output = exec_commands(commands, &shell_result);
    }
    else if( command_method == METHOD_UCI_SET)
    {
        shell_output = exec_commands(commands, &shell_result);
        // Aggiungere il commit (pensare ad un eventuale check)
    }
    else if( command_method == METHOD_SYS_CALL)
    {
        shell_output = exec_commands(commands, &shell_result);
    }
    else
    {
        ; // Skip the commands
    }
    
    
    if(shell_result == 1)
    {
        system_logger(LOGGER_INFO, "SYSTEM", "Command \"%s\" executed correctly", commands);
        system_logger(LOGGER_DEBUG, "SYSTEM", "Shell output of command \"%s\" is: %s", commands, shell_output);
        free(shell_output);
    }    
    else if(shell_result == 0)
    {
        system_logger(LOGGER_ERROR, "SYSTEM", "Command \"%s\" executed with errors", commands);
    } 
    else
    {
        system_logger(LOGGER_ERROR, "SYSTEM", "Command not executed due to unrecognized method");
    }
    

}

static char* exec_commands(char* command, int *result)
{
    char buf[100];
    char *temp = NULL;
    char* output = NULL;
    unsigned int size = 1;  // start with size of 1 to make room for null terminator
    unsigned int strlength;

    FILE *ls;
    if (NULL == (ls = popen(command, "r"))) 
    {
        system_logger(LOGGER_FATAL,"SYSTEM", "Error during popen in the command execution");
        perror("popen");
        (*result) = 0;
    }

   
    while (fgets(buf, sizeof(buf), ls) != NULL) 
    {
        strlength = strlen(buf);
        temp = realloc(output, size + strlength);  // allocate room for the buf that gets appended
        if (temp == NULL) {
            system_logger(LOGGER_FATAL,"SYSTEM", "Error allocation during popen output creation");
            return 0;
        } else {
            output = temp;
        }
        strcpy(output + size - 1, buf);     // append buffer to str
        size += strlength;  
    } 

    if(size>1) (*result) = 1;
    
    pclose(ls);
    return output;
}