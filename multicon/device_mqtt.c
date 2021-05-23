#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <time.h>

#include "device_mqtt.h"
#include "device_json.h"
#include "device_utils.h"
#include "device_settings.h"
#include "system_logger.h"


char topic_to_publish[200];
char topic_to_subscribe[200];
static int ping_log_counter = 0;

bool clean_session = true;
struct mosquitto *mosq = NULL;

void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    bool match = 0;

    if(message->payloadlen)
    {          
        mosquitto_topic_matches_sub(topic_to_subscribe, message->topic, &match);
        if (match) 
        {
            
            system_logger(LOGGER_DEBUG,"MQTT", "Received new message from server %s", (char*) message->payload);

            json_char* json;
            json_value* value;

            json = (json_char*) (char*) message->payload;

            
            value = json_parse(json, strlen( (char*) message->payload) );

            json_handle_message(value);
            
            json_value_free(value);
        }
    }
    fflush(stdout);
}

void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result)
    {
        // Subscribe to broker information topics on successful connect. 
        mosquitto_subscribe(mosq, NULL, topic_to_subscribe, 2);
    }
    else
    {
        system_logger(LOGGER_FATAL,"MQTT", "Connect failed");
    }
}

void mqtt_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    
    system_logger(LOGGER_DEBUG,"MQTT", "Client subscribed (mid: %d): %d", mid, granted_qos[0]);

    for(i=1; i<qos_count; i++)
    {
        printf(", %d", granted_qos[i]);
    }
}

void mqtt_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    // Pring all log messages regardless of level.
    if((ping_log_counter > NUM_OF_LOGGED_PINGS) || (ping_log_counter <= 10))
    {
        system_logger(LOGGER_DEBUG,"MQTT", (char*) str);
        if(ping_log_counter > 10)
            ping_log_counter = 0;
    }
    ping_log_counter++;
}

void mqtt_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    system_logger(LOGGER_WARN,"MQTT", "Module disconnected");
}

int device_mqtt_run()
{
    static int counter = 0;
    int retVal = -1;
    
    static long long int t_cycle = 0;
    static int start_timer = 0;
    
    long long int t_curr;
    
    t_curr = time(NULL);

    if(!start_timer)
    {
        t_cycle = t_curr;
        start_timer = 1;
    }
    
    // Publish a status message every x sec
    if (t_curr - t_cycle >= PUBLISH_TIME_PERIOD)
    {
        char mqtt_message[1000] = {""};
        char timestamp[50]; 
        util_get_timestamp(timestamp);

        #ifdef DEVELOP     
            json_create_json_string(mqtt_message, 32,  
                "timestamp", timestamp,
                "serial", deviceSettings.serial,
                "firmware", deviceSettings.firmware,
                "lan_ip", deviceSettings.lan_ip,
                "lan_netmask", deviceSettings.lan_netmask,
                "lan_proto", deviceSettings.lan_proto,
                "ip_addr", deviceSettings.ip_addr,
                "gsm_rssi_level", deviceSettings.gsm_rssi_level,
                "wcdma_rscp_level", deviceSettings.wcdma_rscp_level,
                "wcdma_ecio_level", deviceSettings.wcdma_ecio_level,
                "lte_rsrp_level", deviceSettings.lte_rsrp_level,
                "lte_sinr_level", deviceSettings.lte_sinr_level,
                "connstate", deviceSettings.connstate,
                "netstate", deviceSettings.netstate,
                "simstate", deviceSettings.simstate,
                "sim_iccd", deviceSettings.sim_iccd);
            retVal = mosquitto_publish (mosq, NULL, topic_to_publish, strlen(mqtt_message), mqtt_message, 0, false); 
            system_logger(LOGGER_INFO,"MQTT", "Published message on topic %s", topic_to_publish); 
            
        #else
            
            json_create_json_string(mqtt_message, 32,  
                "timestamp", timestamp,
                "serial", deviceSettings.serial,
                "firmware", deviceSettings.firmware,
                "lan_ip", deviceSettings.lan_ip,
                "lan_netmask", deviceSettings.lan_netmask,
                "lan_proto", deviceSettings.lan_proto,
                "ip_addr", deviceSettings.ip_addr,
                "gsm_rssi_level", deviceSettings.gsm_rssi_level,
                "wcdma_rscp_level", deviceSettings.wcdma_rscp_level,
                "wcdma_ecio_level", deviceSettings.wcdma_ecio_level,
                "lte_rsrp_level", deviceSettings.lte_rsrp_level,
                "lte_sinr_level", deviceSettings.lte_sinr_level,
                "connstate", deviceSettings.connstate,
                "netstate", deviceSettings.netstate,
                "simstate", deviceSettings.simstate,
                "sim_iccd", deviceSettings.sim_iccd);
            retVal = mosquitto_publish (mosq, NULL, topic_to_publish, strlen(mqtt_message), mqtt_message, 0, false); 
            system_logger(LOGGER_DEBUG,"MQTT", "Published message on topic %s", topic_to_publish);
        #endif
        t_cycle = t_curr;
    }
    return retVal;
}


int device_mqtt_init()
{
    char serial[SETTINGS_SERIAL_LEN] = {'\0'};
    get_device_serial(serial);
    util_snprintf(topic_to_publish, sizeof(topic_to_publish), "multicon/%s",  serial);
    util_snprintf(topic_to_subscribe, sizeof(topic_to_subscribe), "multicon/commands/%s", serial);

    mosquitto_lib_init();

    mosq = mosquitto_new(serial, clean_session, NULL);

    mosquitto_username_pw_set(mosq, CLIENT_USERNAME, CLIENT_PASSWORD);

    if(!mosq){
        system_logger(LOGGER_FATAL,"MQTT", "Out of memory");
        return 1;
    }
    mosquitto_log_callback_set(mosq, mqtt_log_callback);
    mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(mosq, mqtt_message_callback);
    mosquitto_subscribe_callback_set(mosq, mqtt_subscribe_callback);
    mosquitto_disconnect_callback_set(mosq, mqtt_disconnect_callback);

    if(mosquitto_connect_async(mosq, BROKER_HOST, BROKER_PORT, KEEPALIVE_SECONDS)){
        system_logger(LOGGER_ERROR,"MQTT", "Unable to connect");
        return 1;
    }

    if(mosquitto_loop_start(mosq) == MOSQ_ERR_SUCCESS)
    {
        system_logger(LOGGER_INFO,"MQTT", "Module connected and ready");
    }
    else
    {
        system_logger(LOGGER_ERROR,"MQTT", "Unable to start module");
    }
    return 0;
}

int device_mqtt_stop()
{
    system_logger(LOGGER_INFO,"MQTT", "Module is stopped correctly");

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
