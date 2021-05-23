#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "device_json.h"
#include "device_utils.h"
#include "system_logger.h"
#include "device_settings.h"



static int json_get_command_params(json_value* value, char *string_command, int *count)
{
    int i;
    int string_command_len = 0;
    int retVal = 1;
    (*count) = value->u.array.length;
    for(i = 0 ; i<(*count) ; i++)
    {
        char* json_strings = json_get_string(value->u.array.values[i]);
        // Checks if received params is too long
        if(strlen(json_strings) > MAX_COMMAND_STR_LEN)
        {
            retVal = 0;
            break;
        }

        strcat(string_command, json_strings);
        string_command_len = strlen(string_command);

        // Checks if parsing had problems
        if(string_command_len == 0 || string_command_len > MAX_COMMAND_STR_LEN)
        {
            retVal = 0;
            break;
        }
        
        // Check if we need to insert space        
        if( i==( (*count)-1) )
        {
            string_command[string_command_len+1] = '\0';          // followed by terminator
        }
        else
        {
            string_command[string_command_len] = ' ';             // Append a space
        }
    }
    return retVal;
}



void json_handle_message(json_value* value)
{
    int length, x;
    int check_step = 0, parsing_error = 0;
    int exec_result = -1;
    int command_method = -1;

    if (value == NULL) {
        system_logger(LOGGER_ERROR,"JSON", "Unspected json format");
        return;
    }

    length = value->u.object.length;

    // Scorro il json ricevuto incrementando la variabile check_step di volta in volta per verificare che il json sia corretto
    for (x = 0; x < length; x++) 
    {
        if((strcmp(value->u.object.values[x].name, "id")==0) && (check_step == 0))
        {
            // Controllo la coerenza dell'id del comando 
            check_step++;
        }   
        else if((strcmp(value->u.object.values[x].name, "method")==0) && (check_step == 1))
        {
            // Controllo la tipologia di comando dato
            if(strcmp(json_get_string(value->u.object.values[x].value), "uci") == 0)
            {
                command_method = COMMAND_METHOD_UCI;
            }
            else if(strcmp(json_get_string(value->u.object.values[x].value), "call") == 0)
            {
                command_method = COMMAND_METHOD_CALL;
            }  
            else
            {
                system_logger(LOGGER_ERROR,"JSON", "Unrecognized method in command params");
                break;
            }      
            check_step++;
        }  
        else if((strcmp(value->u.object.values[x].name, "token")==0) && (check_step == 2))
        {
            // Verifico il token ricevuto dal server
            check_step++;
        }  
        else if((strcmp(value->u.object.values[x].name, "params")==0) && (check_step == 3))
        {
            // Verifico i parametri per la creazione della stringa da eseguire come comando uci 
            int i;
            int result = 0;
            int num_of_commands;
            char command_params[2000] = {'\0'};
            
            json_value* json_params = value->u.object.values[x].value;
            
            // Controllo se ho una lista di comandi (array di array) oppure un solo comando
            if(json_params->type == json_array)
            {
                if(json_params->u.array.values[0]->type == json_array)
                {
                    // Se ho una lista di comandi, eseguo ciascun comando ricevuto
                    int j;
                    if(JSON_COMMAND_LIST_FUNC)
                    {

                        for (j = 0; j < json_params->u.array.length; j++) 
                        {
                            json_value* json_param_array = json_params->u.array.values[j];
                            
                            result = json_get_command_params(json_param_array, command_params, &num_of_commands);
                            if(result == 1)
                            {
                                device_settings_execute_commands(command_params, command_method);
                            }
                            else
                            {
                                system_logger(LOGGER_ERROR,"JSON", "Command parsing failed");
                            }
                            memset(command_params, 0, sizeof(command_params));
                        }
                    }
                    else
                    {
                        system_logger(LOGGER_WARN,"JSON", "Command list feature is disabled");
                    }
                    
                }
                else
                {
                    // Se ho un solo comando, eseguo il comando ricevuto
                    result = json_get_command_params(json_params,command_params,  &num_of_commands);
                    if(result == 1)
                    {
                        device_settings_execute_commands(command_params, command_method);
                    }
                    else
                    {
                        system_logger(LOGGER_ERROR,"JSON", "Command parsing failed");
                    }
                    memset(command_params, 0, sizeof(command_params));
                } 
            }
            else
            {
                system_logger(LOGGER_ERROR,"JSON", "Unrecognized structure for params in json");
                break;
            }

            check_step++;
        }     
        else
        {
            system_logger(LOGGER_ERROR,"JSON", "Unrecognized json structure");
            break;
        }
                      
    }
    return;
}



static void json_print_array(json_value* value, int depth)
{
    int length, x;
    if (value == NULL) {
        return;
    }
    length = value->u.array.length;
    printf("\tarray\n");
    for (x = 0; x < length; x++) {
        json_print_value(value->u.array.values[x], depth);
    }
}


void json_print_value(json_value* value, int depth)
{
    if (value == NULL) {
        return;
    }
    if (value->type != json_object) {
        print_depth_shift(depth);
    }
    switch (value->type) {
        case json_none:
            printf("\tnone\n");
            break;
        case json_object:
            json_print_object(value, depth+1);
            break;
        case json_array:
            json_print_array(value, depth+1);
            break;
        case json_integer:
            printf("\tint: %10" PRId64 "\n", value->u.integer);
            break;
        case json_double:
            printf("\tdouble: %f\n", value->u.dbl);
            break;
        case json_string:
            printf("\tstring: %s\n", value->u.string.ptr);
            break;
        case json_boolean:
            printf("\tbool: %d\n", value->u.boolean);
            break;
    }
}


static int json_get_integer(json_value* value) 
{
    return value->u.integer;
}

static double json_get_double(json_value* value)
{
    return value->u.dbl;
}

static char* json_get_string(json_value* value)
{
    if(value->type == json_string)
    {
        return value->u.string.ptr;
    }
    else
    {
        system_logger(LOGGER_ERROR,"JSON", "Error in function json_get_string");
        return 0;
    }
}

static int json_get_boolean(json_value* value)
{
    return value->u.boolean;
}

static void print_depth_shift(int depth)
{
    int j;
    for (j=0; j < depth; j++) {
        printf(" ");
    }
}

static void json_print_object(json_value* value, int depth)
{
    int length, x;
    if (value == NULL) {
        return;
    }
    length = value->u.object.length;
    for (x = 0; x < length; x++) {
        print_depth_shift(depth);
        printf("\tobject[%d].name = %s\n", x, value->u.object.values[x].name);
        json_print_value(value->u.object.values[x].value, depth+1);
    }
}

void json_create_json_string(char* dest, int num_args, ...) 
{
  va_list ap;
  int i;
  va_start(ap, num_args);
  
  strcat(dest,"{\n");

  for(i = 0; i < num_args; i++) 
  {
    if((i%2)==0)
    {
      strcat(dest,"\"");
      strcat(dest,va_arg(ap, char*));
      strcat(dest,"\"");
      strcat(dest,":");
    }
    else
    {
      strcat(dest,"\"");
      strcat(dest,va_arg(ap, char*));
      strcat(dest,"\"");
      if(i!=num_args-1)
        strcat(dest,",\n");
      else
        strcat(dest,"\n");
    }
  }
  va_end(ap);
  strcat(dest,"}");
}