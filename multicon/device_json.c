#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "device_json.h"
#include "device_utils.h"
#include "system_logger.h"
#include "device_settings.h"




static char* json_get_command_params(json_value* value, int *count)
{
    char* str = NULL;             // Pointer to the joined strings
    size_t total_length = 0;      // Total length of joined strings
    size_t length = 0;            // Length of a string
    int i = 0;                    // Loop counter

    // Find total length of joined strings 
    (*count) = value->u.array.length;

    for(i = 0 ; i<(*count) ; i++)
    {
        char* strings = json_get_string(value->u.array.values[i]);

        total_length += strlen(strings);
    }
    ++total_length;     // For joined string terminator

    str = (char*)malloc(total_length);  // Allocate memory for joined strings
    str[0] = '\0';                      //Empty string we can append to

    /* Append all the strings */
    for(i = 0 ; i<(*count) ; i++)
    {
        char* strings = json_get_string(value->u.array.values[i]);

        strcat(str, strings);
        length = strlen(str);

        // Check if we need to insert space
        if((str[length-1] != '\n') && ( i!=( (*count)-1) ) )
        {
            str[length] = ' ';             // Append a space
            str[length+1] = '\0';          // followed by terminator
        }
    }
    return str;
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
            //printf("\tobject[%d].name = %s\n", x, value->u.object.values[x].name);
            //json_print_value(value->u.object.values[x].value, 1);
            check_step++;
        }   
        else if((strcmp(value->u.object.values[x].name, "method")==0) && (check_step == 1))
        {
            // Controllo la tipologia di comando dato
            //printf("\tobject[%d].name = %s\n", x, value->u.object.values[x].name);
            //json_print_value(value->u.object.values[x].value, 1);
            if(strcmp(json_get_string(value->u.object.values[x].value), "uci-set") == 0)
            {
                command_method = METHOD_UCI_SET;
            }
            else if(strcmp(json_get_string(value->u.object.values[x].value), "uci-get") == 0)
            {
                command_method = METHOD_UCI_GET;
            }
            else if(strcmp(json_get_string(value->u.object.values[x].value), "call") == 0)
            {
                command_method = METHOD_SYS_CALL;
            }  
            else
            {
                system_logger(LOGGER_ERROR,"JSON", "Unrecognized method in command params");
            }      
            check_step++;
        }  
        else if((strcmp(value->u.object.values[x].name, "token")==0) && (check_step == 2))
        {
            // Verifico il token ricevuto dal server
            //printf("\tobject[%d].name = %s\n", x, value->u.object.values[x].name);
            //json_print_value(value->u.object.values[x].value, 1);
            check_step++;
        }  
        else if((strcmp(value->u.object.values[x].name, "params")==0) && (check_step == 3))
        {
            // Verifico i parametri per la creazione della stringa da eseguire come comando uci 
            
            int i;
            int num_of_commands;
            char* command_params = NULL;
            
            
            command_params = json_get_command_params(value->u.object.values[x].value,  &num_of_commands);

            

            device_settings_execute_commands(command_params, command_method);
            

            check_step++;
        }     
        else
        {
            system_logger(LOGGER_ERROR,"JSON", "Unspected json structure");
            break;
        }
                      
    }
    return;
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