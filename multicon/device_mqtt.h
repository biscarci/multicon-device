
#ifdef DEBUG
#define LOG(...) do { printf(__VA_ARGS__); } while (0)
#else
#define LOG(...)
#endif



#define KEEPALIVE_SECONDS 60 //How many seconds the broker should wait between sending out keep-alive messages

/* Hostname and port for the MQTT broker. */
// #define BROKER_HOST "node02.myqtthub.com" // Hostname for the MQTT broker
#define BROKER_PORT 1883                  // Port for the MQTT broker
#define BROKER_HOST "217.160.175.39"

#ifdef DEVELOP     
    #define CLIENT_ID       "999999"
    #define CLIENT_USERNAME "ubuntu-vm"
    #define CLIENT_PASSWORD "ubuntu-vm"
#else
    #define CLIENT_ID       "787765"
    #define CLIENT_USERNAME "rut995"
    #define CLIENT_PASSWORD "rut995"
#endif


#define NUM_OF_LOGGED_PINGS 5 // Mosquitto pings the broker every 60 sec. Prints log every 30min
#define PUBLISH_TIME_PERIOD 10

int device_mqtt_run();
int device_mqtt_init();
int device_mqtt_stop();