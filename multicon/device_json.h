#include "json.h"


#define MAX_SHELL_OUTPUT_SIZE 5000


static int    json_get_integer(json_value* value);
static double json_get_double(json_value* value);
static char*  json_get_string(json_value* value);
static int    json_get_boolean(json_value* value);
static char*  json_get_command_params(json_value* value, int *count);


// Funzioni per il print del json
void        json_print_value(json_value* value, int depth);
static void print_depth_shift(int depth);
static void json_print_object(json_value* value, int depth);
static void json_print_array(json_value* value, int depth);



void json_handle_message(json_value* value);
