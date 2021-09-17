#define DEVICE_SETTINGS_REFRESH_STATUS_RATE 60
#define DEVICE_SETTINGS_RESET_CMD_STATUS_FLAG 15
#define COMMAND_METHOD_UCI 1
#define COMMAND_METHOD_CALL 2

#define COMMAND_EXECUTED      2
#define COMMAND_EXEC_SUCCESS  1
#define COMMAND_NOT_EXECUTED  0
#define COMMAND_EXEC_ABORT   -1
#define COMMAND_EXEC_FAILED  -2

#define UCI_OP_NOTALLOWED  -1
#define UCI_OP_GET     0
#define UCI_OP_SET     1
#define UCI_OP_ADD     2
#define UCI_OP_SHOW    3
#define UCI_OP_COMMIT  4


#define MAX_SHELL_OUTPUT_LEN 200
#define MAX_SHELL_COMMAND 200

#define SETTINGS_SERIAL_LEN 30
#define SETTINGS_FWVER_LEN 100
#define SETTINGS_IP_LEN 15
#define SETTINGS_PROTO_LEN 15
#define SETTINGS_ADDR_ID 30
#define SETTINGS_JOIN_LEN 30
#define SETTINGS_VPN_LEN 10
#define SETTINGS_GSMCL_SETTINGS 50

typedef struct
{
    char serial[SETTINGS_FWVER_LEN];
    char firmware[SETTINGS_FWVER_LEN];
    char lan_ip[SETTINGS_IP_LEN];
    char lan_netmask[SETTINGS_IP_LEN];
    char lan_proto[SETTINGS_PROTO_LEN];
    char ip_addr[SETTINGS_IP_LEN];
    char gsm_rssi_level[SETTINGS_GSMCL_SETTINGS];
    char wcdma_rscp_level[SETTINGS_GSMCL_SETTINGS];
    char wcdma_ecio_level[SETTINGS_GSMCL_SETTINGS];
    char lte_rsrp_level[SETTINGS_GSMCL_SETTINGS];
    char lte_sinr_level[SETTINGS_GSMCL_SETTINGS];
    char connstate[SETTINGS_GSMCL_SETTINGS];
    char netstate[SETTINGS_GSMCL_SETTINGS];
    char simstate[SETTINGS_GSMCL_SETTINGS];
    char sim_iccd[SETTINGS_GSMCL_SETTINGS];
    int  command_exec_sts;
} t_device_settings_status;

extern t_device_settings_status deviceSettings;




// Command execution
static int uci_commands_validator(char* uci_command);
int exec_shell(char* command, char* shell_output);
int exec_uci(char* command, int op, char* shell_output);

// Utils
static int get_decoded_uci_operation(char* op);

// Public method
void device_settings_run();
void device_settings_init();
void device_settings_execute_commands(char* command, int command_method);
void device_settings_print_status_struct();

int get_device_serial(char* serial);
