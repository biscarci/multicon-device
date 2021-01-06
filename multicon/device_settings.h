


#define COMMAND_METHOD_UCI 1
#define COMMAND_METHOD_CALL 2

#define COMMAND_EXEC_SUCCESS  1
#define COMMAND_EXEC_FAILED   0
#define COMMAND_EXEC_ABORT   -1
#define COMMAND_NOT_EXECUTED -2

#define UCI_OP_GET     0
#define UCI_OP_SET     1
#define UCI_OP_ADD     2
#define UCI_OP_COMMIT  3
#define UCI_OP_NOTALLOWED  -1

// Command execution
static char* exec_shell_commands(char* command);
static int exec_uci_commands(char* uci_command);
static int exec_call_commands(char* command);

// Operation check
static int check_uci_commit_command(char *shell_output);
static int check_uci_add_command(char *shell_output);
static int check_uci_get_command(char *shell_output);
static int check_uci_set_command(char* command_arg, char *shell_output);

// Utils
static int get_decoded_uci_operation(char* op);

// Public method
void device_settings_execute_commands(char* command, int command_method);