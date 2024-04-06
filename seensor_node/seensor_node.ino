#include <DHTesp.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <PubSubClient.h> // PubSubClient library for MQTT communication
#include "sensors.hpp"

const size_t capacity = JSON_OBJECT_SIZE(5);
StaticJsonDocument<capacity> sen_pkt;

const char* ssid = "Airtel_9827349245";
const char* password = "air84579";
const char* mqtt_server = "mqtt.thingsboard.cloud";
const int mqtt_port = 1883; // MQTT default port
WiFiClient espClient;
PubSubClient client(espClient);
const char* topic = "v1/devices/me/telemetry";
const char* mqtt_user = "eblxb7wWsG7JKSxu8JTx";
const int device_id = 1;

// Sensor data
extern SensorData sensor_data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  // Setup all sensor hardware
  setup_sensors ();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("06b5c6c0-f419-11ee-ae87-79b197dbfe12", mqtt_user, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void send_sensor_data (){
  // Start WiFi in Station (STA) Mode
  delay(10);
  Serial.println("Starting WiFi in Station Mode");
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Setup Rabbit MQ server connection
  client.setServer(mqtt_server, mqtt_port);
  // Read the sensors
  read_all_sensors ();

  // Checkup on MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Preparing the packet
  sen_pkt["dvc_id"] = device_id;
  sen_pkt["dht_temp"] = sensor_data.dht11_temperature;
  sen_pkt["dht_hum"] = sensor_data.dht11_humidity;
  sen_pkt["light_int"] = sensor_data.light_intensity;
  sen_pkt["temp"] = sensor_data.temperature;
  sen_pkt["current"] = sensor_data.current;
  String sen_pkt_str;
  serializeJson(sen_pkt, sen_pkt_str);

  // Checkup on MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish JSON data to a specific MQTT topic
  Serial.print ("Seending this Packet: ");
  Serial.println (sen_pkt_str);
  client.publish(topic, sen_pkt_str.c_str());
    // Checkup on MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Disconnect and close MQTT client
  if (client.connected()) {
    client.disconnect();
  }
  // Close WiFi client
  espClient.stop();
  // Disable WiFi
  Serial.println("Stopping WiFi");
  WiFi.mode(WIFI_OFF);
}

void wake_routine (){
  // Read all the sensor data
  read_all_sensors ();
  // Call send data
  send_sensor_data ();
}

void loop() {
  // put your main code here, to run repeatedly:


  // Call wake routine
  wake_routine ();
  // Set sleep time
  esp_sleep_enable_timer_wakeup(20*1000000); // Takes value in uS so we give value as S*1000000
  // Go to deep sleep
  esp_deep_sleep_start();
}
