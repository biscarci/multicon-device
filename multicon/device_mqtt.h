
#ifdef DEBUG
#define LOG(...) do { printf(__VA_ARGS__); } while (0)
#else
#define LOG(...)
#endif


#define KEEPALIVE_SECONDS 60 //How many seconds the broker should wait between sending out keep-alive messages

/* Hostname and port for the MQTT broker. */
#define BROKER_HOST "node02.myqtthub.com" // Hostname for the MQTT broker
#define BROKER_PORT 1883                  // Port for the MQTT broker

#define TOPIC       "topic/rut955"
#define PAYLOAD     "Hello World!"

#define CLIENT_ID    "787765"
#define CLIENT_USERNAME "rut955"
#define CLIENT_PASSWORD "rut955"

#define NUM_OF_LOGGED_PINGS 5 // Mosquitto pings the broker every 60 sec. Prints log every 30min

int device_mqtt_run();
int device_mqtt_init();
int device_mqtt_stop();