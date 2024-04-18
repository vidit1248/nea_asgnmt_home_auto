#include <DHTesp.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <PubSubClient.h> // PubSubClient library for MQTT communication

//#define WIFI_AP "Airtel_9827349245"
//#define WIFI_PASSWORD "air84579"
#define WIFI_AP "Airtel_DATTEBAYOH"
#define WIFI_PASSWORD "dattebayoh"

#define TOKEN "oK7qUQtSBzEOZ8HKKSPB"
#define DEV_ID "d1e0dba0-fa31-11ee-80ac-39b6c0e0bad3"

#define GPIO0 19
#define GPIO2 18

#define GPIO0_PIN 1
#define GPIO2_PIN 2

char thingsboardServer[] = "mqtt.thingsboard.cloud";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

// We assume that all GPIOs are LOW
boolean gpioState[] = {false, false};

void setup() {
  Serial.begin(115200);
  // Set output mode for all GPIO pins
  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  client.loop();
  delay (2000);
}

// The callback for when a PUBLISH message is received from the server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("On message");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonDocument<200> data;
  DeserializationError error = deserializeJson(data, json);

+   Serial.println(error.c_str());
  if (error)
  {
    Serial.print("deserializeJson() error returned ");
+   Serial.println(error.c_str());
    return;
  }

  // Check request method
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getGpioStatus")) 
  {
    // Reply with GPIO status
    Serial.print("deserializeJson() returned ");
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } 
  else if (methodName.equals("setGpioStatus")) 
  {
    // Update GPIO status and reply
    set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonDocument<200> jsonBuffer;
  String strPayload;
  jsonBuffer[String(GPIO0_PIN)] = gpioState[0] ? true : false;
  jsonBuffer[String(GPIO2_PIN)] = gpioState[1] ? true : false;
  serializeJson (jsonBuffer, strPayload);
  Serial.print("Get gpio status: ");
  Serial.println(strPayload);
  return strPayload;
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO0, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[0] = enabled;
  } else if (pin == GPIO2_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO2, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[1] = enabled;
  }
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("d1e0dba0-fa31-11ee-80ac-39b6c0e0bad3", "oK7qUQtSBzEOZ8HKKSPB", NULL) ) {
      Serial.println( "[DONE]" );
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      // Sending current GPIO status
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/telemetry", get_gpio_status().c_str());
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
