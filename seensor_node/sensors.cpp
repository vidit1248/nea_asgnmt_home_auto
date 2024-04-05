#include "sensors.hpp"

SensorData sensor_data;
DHT dht(DHTPIN, DHTTYPE);

const int light_sensor_pin = 36, temp_sensor_pin = 39, current_sensor_pin = 34;

void setup_sensors (){
  // Setup DHT11
  pinMode (DHT_EN_PIN, INPUT_PULLUP);
  if (digitalRead (DHT_EN_PIN)) dht.begin ();
}

void read_DHT11 (int * temp, int * humd){
  if (digitalRead (DHT_EN_PIN)){
    dht.begin();
    Serial.println("Reading DHT11");
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    *humd = dht.readHumidity();
    // Read temperature as Celsius (the default)
    *temp = dht.readTemperature();

    Serial.print(F("Humidity: "));
    Serial.print(*humd);
    Serial.print(F("%  Temperature: "));
    Serial.print(*temp);
    Serial.println(F("°C "));
    //dht.end();
  }
  else{
    Serial.println("DHT11 Disbled from Hardware Pin");
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    *humd = 0;
    // Read temperature as Celsius (the default)
    *temp = 0;
    Serial.print(F("Humidity: "));
    Serial.print(*humd);
    Serial.print(F("%  Temperature: "));
    Serial.print(*temp);
    Serial.println(F("°C "));
  }
}

void read_all_sensors (){
  read_DHT11 (&(sensor_data.dht11_temperature), &(sensor_data.dht11_humidity));
  sensor_data.light_intensity = analogRead (light_sensor_pin);
  Serial.print ("Read Light Sensor: "); Serial.println (sensor_data.light_intensity);
  sensor_data.temperature = analogRead (temp_sensor_pin);
  Serial.print ("Read Light Temprature: "); Serial.println (sensor_data.temperature);
  sensor_data.current = analogRead (current_sensor_pin);
  Serial.print ("Read Light Current: "); Serial.println (sensor_data.current);
}