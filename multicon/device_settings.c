#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include "device_settings.h"
#include "device_utils.h"
#include "system_logger.h"

t_device_settings_status deviceSettings;


/**
 * Funzione che viene chiamata per eseguire un comando a shell che può essere uci o una classica chiamata
 */
void device_settings_execute_commands(char* commands, int command_method)
{
    int exec_status = COMMAND_NOT_EXECUTED;
    char shell_output[MAX_SHELL_OUTPUT_LEN];
    
    if( command_method == COMMAND_METHOD_UCI)
    {
        exec_status = uci_commands_validator(commands);
    }
    else if( command_method == COMMAND_METHOD_CALL)
    {
        exec_status = exec_shell(commands, shell_output);
        system_logger(LOGGER_DEBUG, "SYSTEM", "Shell command %s output: %s", commands, shell_output);
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


static int uci_commands_validator(char* uci_command)
{
    int uci_operation;
    int exec_status = COMMAND_NOT_EXECUTED;
    char** params_command_array;
    
    // Split della stringa dei comandi
    params_command_array = util_strsplit(uci_command, " ");
   
    // Se lo split va a buon fine, effettuo il controllo della sintassi prima di eseguire il comando
    if (params_command_array)
    {
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

        // Se il processo di validazione è superato, eseguo il comando uci
        if(exec_status != COMMAND_EXEC_ABORT)
        {
            char shell_output[MAX_SHELL_OUTPUT_LEN] = {'\0'};
            // Se il controllo della sintassi è andato a buon fine, eseguo il comando
            exec_status = exec_uci(uci_command, uci_operation, shell_output);
            system_logger(LOGGER_DEBUG, "SYSTEM", "Uci command %s output: %s", uci_command, shell_output);
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


/**
 * Funzione usata per eseguire un qualsiasi uci da shell
 */
int exec_uci(char* command, int op, char* shell_output)
{
    FILE *fd;
    char c[MAX_SHELL_COMMAND];

    if(op == UCI_OP_GET || op == UCI_OP_SHOW)
    {
        util_snprintf(c, sizeof(c), "%s | sed -e \"s/.*=// ; s/['\\\"]//g\"", command);
    }
    else
    {
        util_snprintf(c, sizeof(c), "%s", command);
    }

    fd = popen(c, "r");

    if(fd == NULL) 
    {
        system_logger(LOGGER_WARN,"SYSTEM", "Could not open pipe, the command isn't executed");
        return COMMAND_EXEC_ABORT;
    }

    // Read process output
    fgets(shell_output, MAX_SHELL_OUTPUT_LEN, fd);
    util_removechar(shell_output, 10); // Rimozione dello \n (In ASCII 10)
    pclose(fd);
    return COMMAND_EXEC_SUCCESS;
}

/**
 * Funzione usata per eseguire un qualsiasi comando da shell
 */
int exec_shell(char* command, char* shell_output)
{

    FILE *fd = popen(command, "r");

    if(fd == NULL) 
    {
        system_logger(LOGGER_WARN,"SYSTEM", "Could not open pipe, the command isn't executed");
        return COMMAND_EXEC_ABORT;
    }
    // Read process output
    fgets(shell_output, MAX_SHELL_OUTPUT_LEN, fd);
    util_removechar(shell_output, 10); // Rimozione dello \n (In ASCII 10)
    pclose(fd);
    return COMMAND_EXEC_SUCCESS;
}

/**
 * FunzionUcie che consente di ottenere il 
 * corrispettivo codificato dell'operazione uci specificata
 */
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
    else if(strcmp( op, "show") == 0)
    {
        return UCI_OP_SHOW;
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

/**
 * Ritorna il seriale del device
 */
int get_device_serial(char* serial)
{        
    int res;
    #ifdef DEVELOP
        util_snprintf(serial, sizeof(serial), "%s", "99999999999" );
        system_logger(LOGGER_WARN, "SYSTEM", "Multicon app is launched in develop-mode assuming serial %s", serial);
        res = COMMAND_EXEC_SUCCESS;    
    #else
        res = exec_uci("uci show hwinfo.hwinfo.serial", UCI_OP_SHOW, serial);
    #endif
    
    return res;
}

static int check_zerotier_process_status()
{
    char output[50] = {'\0'};
    exec_shell("zerotier-cli info", output);
    if(strstr(output, "OFFLINE") !=  NULL)
    {
        system_logger(LOGGER_WARN, "SYSTEM", "Zerotier app is offline, now it will be restarted (%s)", output);
        exec_shell("/etc/init.d/zerotier restart", output);
    }
}

int device_settings_refresh_status()
{
    // Get device firmware version
    exec_uci("uci show system.system.device_fw_version ", UCI_OP_SHOW, deviceSettings.firmware);

    // Get device serial
    exec_uci("uci show hwinfo.hwinfo.serial", UCI_OP_SHOW, deviceSettings.serial);

    // Get network lan ipaddr
    exec_uci("uci show network.lan.ipaddr", UCI_OP_SHOW, deviceSettings.lan_ip);

    // Get network lan netmask
    exec_uci("uci show network.lan.netmask", UCI_OP_SHOW, deviceSettings.lan_netmask);

    // Get network lan proto
    exec_uci("uci show network.lan.proto", UCI_OP_SHOW, deviceSettings.lan_proto);

    // Get zerotier id
    exec_uci("uci show zerotier.zerotier.address", UCI_OP_SHOW, deviceSettings.zerotier_id);

    // Get zerotier join
    exec_uci("uci show zerotier.zerotier.join", UCI_OP_SHOW, deviceSettings.zerotier_join);

    // Get zerotier vpn enabled
    exec_uci("uci show zerotier.zerotier.vpnenabled", UCI_OP_SHOW, deviceSettings.zerotier_vpnenabled);

    // Get ip addr
    exec_shell("ip addr show zt0 | sed -Ene \'s/^.*inet ([0-9.]+)\\/.*$/\\1/p\'", deviceSettings.ip_addr);

    // To obtain the registration state of the mobile network
    exec_shell("gsmctl -z", deviceSettings.simstate);

    // To obtain the network connection state
    exec_shell("gsmctl -j", deviceSettings.connstate); 

    // To obtain the registration state of the mobile network
    exec_shell("gsmctl -g", deviceSettings.netstate); 

    /** To obtain the router's current signal strength (RSSI) value
     *  2G and 3G signal levels (dBm)
     *    >= -70       Excellent  Strong signal with maximum data speeds
     *    -70 to -85   Good       Strong signal with good data speeds
     *    -86 to -100  Fair       Fair but useful
     *    < -100       Poor       Performance will drop drastically
     *    -110         No signal  Disconnection
     *  4G signal levels
     *    > -65        Excellent  Strong signal with maximum data speeds
     *    -65 to -75   Good       Strong signal with good data speeds
     *    -75 to -85   Fair       Fair but useful
     *    -85 to -95   Poor       Performance will drop drastically
     *    <= -95       No signal  Disconnection 
     */
    exec_shell("gsmctl -q", deviceSettings.gsm_rssi_level);

    /** To obtain the router's current WCDMA RSCP level, RSCP Signal strength description:
     *  -60 to 0    Excellent	Strong signal with maximum data speeds
     *  -75 to -60  Good	    Strong signal with good data speeds
     *  -85 to -75  Fair	    Fair but useful, fast and reliable data speeds may be attained
     *  -95 to -85  Poor	    Marginal data with drop-outs is possible
     *  -124 to -95 Very poor	Performance will drop drastically, closer to -124 disconnects are likely
     */
    exec_shell("gsmctl -X", deviceSettings.wcdma_rscp_level);

    /** To obtain the router's current WCDMA EC/IO level, use -E or --ecio options. EC/IO	Signal quality	Description
     *  0 to -6    Excellent   Strong signal with maximum data speeds
     *  -7 to -10  Good        Strong signal with good data speeds
     *  -11 to -20 Fair        to poor Reliable data speeds may be attained
     */
    exec_shell("gsmctl -E", deviceSettings.wcdma_ecio_level);

    /** To obtain the router's current LTE RSRP level, RSRP Signal strength description (dBm)
     *  >= -80      Excellent	Strong signal with maximum data speeds
     *  -80 to -90  Good	    Strong signal with good data speeds
     *  -90 to -100	Fair        to poor	Reliable data speeds may be attained
     *  <= -100     No signal	Disnonnection
     */
    exec_shell("gsmctl -W", deviceSettings.lte_rsrp_level);

    /** To obtain the router's current SINR level, SINR Signal strength description (dB)
     *  >= 20       Excellent    Strong signal with maximum data speeds
     *  13 to 20    Good         Strong signal with good data speeds
     *  0 to 13     Fair         to poor Reliable data speeds may be attained
     *  <= 0        No           signal Disconnection
     */
    exec_shell("gsmctl -Z", deviceSettings.lte_sinr_level);

    // To obtain SIM ICCD
    exec_shell("gsmctl -J", deviceSettings.sim_iccd);

}


void device_settings_print_status_struct()
{
    printf("--------------------------------------------------------\n");
    printf("              serial: %s\n", deviceSettings.serial);
    printf("            firmware: %s\n", deviceSettings.firmware);
    printf("              lan_ip: %s\n", deviceSettings.lan_ip);
    printf("         lan_netmask: %s\n", deviceSettings.lan_netmask);
    printf("           lan_proto: %s\n", deviceSettings.lan_proto);
    printf("         zerotier_id: %s\n", deviceSettings.zerotier_id);
    printf("       zerotier_join: %s\n", deviceSettings.zerotier_join);
    printf(" zerotier_vpnenabled: %s\n", deviceSettings.zerotier_vpnenabled);
    printf("             ip_addr: %s\n", deviceSettings.ip_addr);
    printf("      gsm_rssi_level: %s\n", deviceSettings.gsm_rssi_level);
    printf("    wcdma_rscp_level: %s\n", deviceSettings.wcdma_rscp_level);
    printf("    wcdma_ecio_level: %s\n", deviceSettings.wcdma_ecio_level);
    printf("      lte_rsrp_level: %s\n", deviceSettings.lte_rsrp_level);
    printf("      lte_sinr_level: %s\n", deviceSettings.lte_sinr_level);
    printf("           connstate: %s\n", deviceSettings.connstate);
    printf("            netstate: %s\n", deviceSettings.netstate);
    printf("            simstate: %s\n", deviceSettings.simstate);
    printf("            sim_iccd: %s\n", deviceSettings.sim_iccd);
    printf("--------------------------------------------------------\n");
}


void device_settings_run()
{
    static long long int t_cycle = 0;
    
    long long int t_curr;
    
    t_curr = time(NULL);

    if (t_curr - t_cycle >= DEVICE_SETTINGS_REFRESH_STATUS_RATE)
    {   
        #ifdef DEVELOP 
            ;//device_settings_print_status_struct();
        #else
            device_settings_refresh_status();
            check_zerotier_process_status();
            //device_settings_print_status_struct();
        #endif
        
        t_cycle = t_curr;
    }
}

void device_settings_init()
{
    #ifdef DEVELOP 
        system_logger(LOGGER_WARN, "SYSTEM", "Multicon started in develop-mode the device setting status is in test-mode");
        util_snprintf(deviceSettings.serial, sizeof(deviceSettings.serial), "%s", "1108785374");
        util_snprintf(deviceSettings.firmware, sizeof(deviceSettings.firmware), "%s", "RUT9XX_R_GPL_00.06.07.5");
        util_snprintf(deviceSettings.lan_ip, sizeof(deviceSettings.lan_ip), "%s", "192.168.8.1");
        util_snprintf(deviceSettings.lan_netmask, sizeof(deviceSettings.lan_netmask), "%s", "255.255.255.0");
        util_snprintf(deviceSettings.lan_proto, sizeof(deviceSettings.lan_proto), "%s", "static");
        util_snprintf(deviceSettings.zerotier_id, sizeof(deviceSettings.zerotier_id), "%s", "e0ba504a4e");
        util_snprintf(deviceSettings.zerotier_join, sizeof(deviceSettings.zerotier_join), "%s", "159924d6307a290e");
        util_snprintf(deviceSettings.zerotier_vpnenabled, sizeof(deviceSettings.zerotier_vpnenabled), "%s", "1");
        util_snprintf(deviceSettings.ip_addr, sizeof(deviceSettings.ip_addr), "%s", "10.147.18.225");
        util_snprintf(deviceSettings.gsm_rssi_level, sizeof(deviceSettings.gsm_rssi_level), "%s", "-73");
        util_snprintf(deviceSettings.wcdma_rscp_level, sizeof(deviceSettings.wcdma_rscp_level), "%s", "service mode not supported");
        util_snprintf(deviceSettings.wcdma_ecio_level, sizeof(deviceSettings.wcdma_ecio_level), "%s", "service mode not supported");
        util_snprintf(deviceSettings.lte_rsrp_level, sizeof(deviceSettings.lte_rsrp_level), "%s", "-110");
        util_snprintf(deviceSettings.lte_sinr_level, sizeof(deviceSettings.lte_sinr_level), "%s", "-1.1");
        util_snprintf(deviceSettings.connstate, sizeof(deviceSettings.connstate), "%s", "connected");
        util_snprintf(deviceSettings.netstate, sizeof(deviceSettings.netstate), "%s", "registered (home)");
        util_snprintf(deviceSettings.simstate, sizeof(deviceSettings.simstate), "%s", "inserted");
        util_snprintf(deviceSettings.sim_iccd, sizeof(deviceSettings.sim_iccd), "%s", "8939109600013842511F");
    #else
        device_settings_refresh_status();
    #endif
}