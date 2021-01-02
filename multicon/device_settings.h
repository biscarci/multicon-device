


#define METHOD_UCI_GET 1
#define METHOD_UCI_SET 2
#define METHOD_SYS_CALL 3


static char* exec_commands(char* command, int *result);


int device_settings_execute_commands(char* command, int command_method);