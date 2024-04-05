#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11
#define DHT_EN_PIN   6

struct SensorData {
    int dht11_temperature;    // Temperature from DHT11 sensor
    int dht11_humidity;       // Humidity from DHT11 sensor
    int light_intensity;      // Light intensity from analog light sensor
    int temperature;          // Temperature from analog temperature sensor
    int current;              // Current from ACS712 sensor
};

void setup_sensors ();
void read_all_sensors ();