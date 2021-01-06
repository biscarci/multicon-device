#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "device_settings.h"
#include "device_utils.h"
#include "system_logger.h"



void device_settings_execute_commands(char* commands, int command_method)
{
    int exec_status = COMMAND_NOT_EXECUTED;

    if( command_method == COMMAND_METHOD_UCI)
    {
        exec_status = exec_uci_commands(commands);
    }
    else if( command_method == COMMAND_METHOD_CALL)
    {
        exec_status = exec_call_commands(commands);
    }
    else
    {
        system_logger(LOGGER_ERROR, "SYSTEM", "Command method unrecognized, (%s) has not been executed", commands);
    }
    
    switch (exec_status)
    {
        case COMMAND_NOT_EXECUTED:
            system_logger(LOGGER_ERROR, "SYSTEM", "Command \"%s\" has not been executed (error in the method)", commands);
            break;

        case COMMAND_EXEC_SUCCESS:
            system_logger(LOGGER_INFO, "SYSTEM", "Command (%s) executed correctly", commands);
            break;
        
        case COMMAND_EXEC_FAILED:
            system_logger(LOGGER_ERROR, "SYSTEM", "Command (%s) executed with errors", commands);
            break;
        
        case COMMAND_EXEC_ABORT:
            system_logger(LOGGER_ERROR, "SYSTEM", "Command (%s) has been aborted", commands);
            break;
        
        default:
            break;
    }
}

static int exec_uci_commands(char* uci_command)
{
    int uci_operation;
    int exec_status = COMMAND_NOT_EXECUTED;
    char** params_command_array;
    

    // Split della stringa dei comandi
    params_command_array = util_strsplit(uci_command, " ");
   
    // Se lo split va a buon fine, effettuo il controllo della sintassi prima di eseguire il comando
    if (params_command_array)
    {
        int i;

        // Controllo se il comando uci è stato specificato correttamente
        if(strcmp( params_command_array[0], "uci") != 0)
        {
            system_logger(LOGGER_ERROR, "SYSTEM", "Wrong sintax for uci command (%s), the operation will not executed", params_command_array[0]);
            exec_status = COMMAND_EXEC_ABORT;
        }

        // Controllo se è stato specificato correttamente l'operazione di set, get, commit e add (le altre operazioni sono disabilitate)
        uci_operation = get_decoded_uci_operation(params_command_array[1]);

        if(uci_operation == UCI_OP_NOTALLOWED)
        {
            system_logger(LOGGER_ERROR, "SYSTEM", "Unallowed operation for uci command (%s), the operation will not executed", params_command_array[1]);
            exec_status = COMMAND_EXEC_ABORT;
        }

        if(exec_status != COMMAND_EXEC_ABORT)
        {
            char *shell_output = NULL;

            // Se il controllo della sintassi è andato a buon fine, eseguo il comando
            shell_output = exec_shell_commands(uci_command);

            // Se la set va a buon fine, l'output della shell è vuoto
            switch (uci_operation)
            {
                case UCI_OP_SET:
                    exec_status = check_uci_set_command(params_command_array[2], shell_output);
                    break;
                
                case UCI_OP_GET:
                    exec_status = check_uci_get_command(shell_output);
                    break;
                
                case UCI_OP_ADD:
                    exec_status = check_uci_add_command(shell_output);
                    break;
                
                case UCI_OP_COMMIT:
                    exec_status = check_uci_commit_command(shell_output);
                    break;
            }

            free(shell_output);
        }          
        
        // Pulisco la memoria allocata dinamicamente per la variabile params_command_array
        free(params_command_array);
    }
    else
    {
        system_logger(LOGGER_ERROR, "SYSTEM", "Wrong sintax for uci command (%s), an error is occurred during the splitting process", uci_command );
    }
    return exec_status;
}

static int check_uci_commit_command(char *shell_output)
{
    if(shell_output == NULL)
        return COMMAND_EXEC_SUCCESS;
    else
        return COMMAND_EXEC_FAILED;
}

static int check_uci_add_command(char *shell_output)
{
    if(shell_output == NULL)
        return COMMAND_EXEC_SUCCESS;
    else
        return COMMAND_EXEC_FAILED;
}

static int check_uci_get_command(char *shell_output)
{
    if(shell_output != NULL)
        return COMMAND_EXEC_SUCCESS;
    else
        return COMMAND_EXEC_FAILED;
}

/* Check dell'operazione di set, 
effettuo il check sulla sintassi dell'argomento e 
sull'effettiva modifica della sezione */
static int check_uci_set_command(char* command_arg, char *shell_output)
{
    int result = COMMAND_EXEC_FAILED;
    int i;

    if(shell_output != NULL)
    {
        system_logger(LOGGER_ERROR, "SYSTEM", "Set uci command failed, the operation has been executed but has failed" );
        system_logger(LOGGER_DEBUG, "SYSTEM", "Set uci command failed, the shell output is: %s", shell_output );
    }
    else
    {   
        char command_check_str[200];
        char** arguments_array;

        // Splitto gli argomenti del comando rispetto al simbolo '='
        arguments_array = util_strsplit(command_arg, "=");

        if (arguments_array)
        {
            char *shell_output_check = NULL;
            // Ricostruisco il comando ma questa volta con la get
            util_snprintf(command_check_str, sizeof(command_check_str), "uci get %s", arguments_array[0] );

            shell_output_check = util_removechar( exec_shell_commands(command_check_str), 10); // In ASCII 10 corrisponde a \n
            
            if(shell_output_check)
            {
                // Verifico che argomento specificato nella set coincida con quello della get
                if(strcmp( shell_output_check, util_removechar(arguments_array[1], '\'') ) == 0)
                {
                    result = COMMAND_EXEC_SUCCESS;
                }
                free(shell_output_check);
            }
            else
            {
                system_logger(LOGGER_DEBUG, "SYSTEM", "No output from shell during the check process of set command", shell_output );
            }            
        }

        // Pulisco la memoria allocata dinamicamente
        free(arguments_array);
        
    }
    return result;
}

static int exec_call_commands(char* command)
{
    char** params_command_array;
    int exec_status = COMMAND_EXEC_SUCCESS;
    // Split della stringa dei comandi
    params_command_array = util_strsplit(command, " ");

    if (params_command_array)
    {
        int i;

        // Controllo se il comando uci è stato specificato correttamente
        if(strcmp( params_command_array[0], "uci") == 0)
        {
            system_logger(LOGGER_ERROR, "SYSTEM", "Unauthorized uci command (%s) with call method, the operation will not executed", command);
            exec_status = COMMAND_EXEC_ABORT;
        }
        free(params_command_array);
    }

    exec_shell_commands(command);
    return exec_status;   
}

static char* exec_shell_commands(char* command)
{
    char buf[100];
    char *temp = NULL;
    char* output = NULL;
    unsigned int size = 1;  // start with size of 1 to make room for null terminator
    unsigned int strlength;

    FILE *ls;
    if (NULL == (ls = popen(command, "r"))) 
    {
        system_logger(LOGGER_FATAL,"SYSTEM", "Error during popen in the commands execution");
        perror("popen");
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
    
    pclose(ls);
    return output;
}

static int get_decoded_uci_operation(char* op)
{
    if(strcmp( op, "set") == 0)
    {
        return UCI_OP_SET;
    }
    else if(strcmp( op, "get") == 0)
    {
        return UCI_OP_GET;
    }
    else if(strcmp( op, "add") == 0)
    {
        return UCI_OP_ADD;
    }
    else if(strcmp( op, "commit") == 0)
    {
        return UCI_OP_COMMIT;
    }
    else
    {
        return UCI_OP_NOTALLOWED;
    }
}