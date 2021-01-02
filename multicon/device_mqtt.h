/* The linked code creates a client that connects to a broker at
 * localhost:1883, subscribes to the topics "tick", "control/#{PID}",
 * and "control/all", and publishes its process ID and uptime (in
 * seconds) on "tock/#{PID}" every time it gets a "tick" message. If the
 * message "halt" is sent to either "control" endpoint, it will
 * disconnect, free its resources, and exit. */

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


int device_mqtt_run();
int device_mqtt_init();
int device_mqtt_stop();