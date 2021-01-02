#include <stdio.h>
#include <mosquitto.h>

#include "device_mqtt.h"
#include "device_json.h"
#include "device_utils.h"
#include "system_logger.h"




bool clean_session = true;
struct mosquitto *mosq = NULL;

void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    bool match = 0;

    if(message->payloadlen)
    {          
        mosquitto_topic_matches_sub("topic/", message->topic, &match);
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
        mosquitto_subscribe(mosq, NULL, "topic/", 2);
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
    system_logger(LOGGER_DEBUG,"MQTT", (char*) str);
}


int device_mqtt_run()
{

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

int device_mqtt_init()
{

    mosquitto_lib_init();

    mosq = mosquitto_new(CLIENT_ID, clean_session, NULL);

    mosquitto_username_pw_set(mosq, CLIENT_USERNAME, CLIENT_PASSWORD);

    if(!mosq){
        system_logger(LOGGER_FATAL,"MQTT", "Out of memory");
        return 1;
    }
    mosquitto_log_callback_set(mosq, mqtt_log_callback);
    mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(mosq, mqtt_message_callback);
    mosquitto_subscribe_callback_set(mosq, mqtt_subscribe_callback);

    return 0;
}

int device_mqtt_stop()
{
    system_logger(LOGGER_INFO,"MQTT", "Module is stopped correctly");

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}