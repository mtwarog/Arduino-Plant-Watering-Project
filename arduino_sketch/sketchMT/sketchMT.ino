#include "dht.h"
#define dht_dpin A3
dht DHT;

// TODO export here port of server and number of characters in http request
const String WIFI_SSID = "P0licja";
const String WIFI_PASS = "szanieckiego68";
//const String SERVER_IP = "192.168.1.4";
//const String SERVER_IP = "192.168.43.40";
const String SERVER_IP = "137.74.174.204";
//const String SERVER_IP = "mactwa.pl"
const int WATERPUMP_DRIVER = 10;

// Sensors characteristics
// pins
int soil_hum = 0;
int lum = A5;
//int air_temp_hum = A3;
// consts
int soil_hum_min = 0;
int soil_hum_max = 1023;

bool wifiConnectionOnline;
float sensorsData[4];

void setup()
{
    Serial.begin(115200); // Communication with ESP8266
    pinMode(LED_BUILTIN, OUTPUT); // Initialize builtin LED. Needed to indicate if wifi is connected.
    digitalWrite(LED_BUILTIN, LOW); // Set initialiy LED off.
    pinMode(WATERPUMP_DRIVER, OUTPUT); // Set water pump relay driver on pin.
    digitalWrite(WATERPUMP_DRIVER, LOW); // Set initially water pump off.
    
    delay(5000);
    wifiConnectionOnline = connectToNetwork();
}
void loop()
{  
  // Initialize wifi
  // Loop:
  //    Evey 1min:
  //      Pull from server if pump should be turned on
  //      If yes turn pump on for few sec
  //    Every 2min:    
  //      Read sensors
  //      Send sensor reads
  // Exception handling:
  //    Check if connection with internet exists (ping something every 10min)
  //    If not try to reconnect
  
  while(wifiConnectionOnline) {
      readSensors(); // Read data
      sendDataToServer(); // Send data to server
      Serial.setTimeout(2000);
      String output = Serial.readString();
      if (output.indexOf("RUN_PUMP") > -1) {
         pumpWater(1000);
      }
      Serial.println("AT+CIPCLOSE"); // Close connection with server
      
      // TODO Check if wifi connection still available - if not exit loop and try to reconnect.
      Serial.setTimeout(1000);
      Serial.readString();
      Serial.println("AT+CIPSTATUS");
      delay(500);
      if (Serial.available() > 0) {
        String status_output = Serial.readString();
        if (status_output.indexOf("STATUS:5") > -1) {
           wifiConnectionOnline = false;
           digitalWrite(LED_BUILTIN, LOW); 
        }
      }
      Serial.setTimeout(2000);
      // TODO If wifi connection is ofline turn off LED (PIN 13)
      delay(20000);  
      // TODO Every 30sec check if pump should be turned on. Wait two minutes for next reads.
      
  }
  wifiConnectionOnline = connectToNetwork();
}
// Connect to wifi network. Return true if connected, false otherwise.
bool connectToNetwork() {
  Serial.write("AT+CWMODE=3\r\n"); // Set wifi module to client + AP mode
  delay(500);
  char setWiFiCommand[100];
  String("AT+CWJAP=\"" + WIFI_SSID +  "\",\"" + WIFI_PASS + "\"\r\n").toCharArray(setWiFiCommand,100);
  Serial.write(setWiFiCommand); // Connect to wifi network (TODO change it to my phone network)
  // TODO check if connected to network
  Serial.setTimeout(8000);
  String output = Serial.readString();
  if (output.indexOf("OK") > -1) {
     digitalWrite(LED_BUILTIN, HIGH); // TODO turn led on ONLY if connected to network
     return true;
  }
  return false;
}

// Read all sensors
void readSensors () {
  DHT.read11(dht_dpin);
  delay(200);
  //sensorsData[0] = readAirTemp();
  //sensorsData[1] = readAirHum();
  sensorsData[0] = DHT.temperature;
  sensorsData[1] = DHT.humidity;
  delay(200);
  sensorsData[2] = readSoilHum();
  delay(200);
  sensorsData[3] = readLightInt();
  delay(200);
}

// Sends read sensors data to server
void sendDataToServer () {
  String luminocity;
  switch (String(sensorsData[3],1).length()) {
    case 3:
      //do something when var equals 1
      luminocity = String("0") + String(sensorsData[3],1).substring(0,3);
      break;
    default: 
      luminocity = String(sensorsData[3],1).substring(0,4);
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  String air_temperature;
  switch (String(sensorsData[0],1).length()) {
    case 3:
      //do something when var equals 1
      air_temperature = String("0") + String(sensorsData[0],1).substring(0,3);
      break;
    default: 
      air_temperature = String(sensorsData[0],1).substring(0,4);
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  String air_humidity;
  switch (String(sensorsData[1],1).length()) {
    case 3:
      //do something when var equals 1
      air_humidity = String("0") + String(sensorsData[1],1).substring(0,3);
      break;
    default: 
      air_humidity = String(sensorsData[1],1).substring(0,4);
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  String soil_hum;
  switch (String(sensorsData[2],1).length()) {
    case 3:
      //do something when var equals 1
      soil_hum = String("0") + String(sensorsData[2],1).substring(0,3);
      break;
    default: 
      soil_hum = String(sensorsData[2],1).substring(0,4);
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  
  char tcpHeader[50];
  (String("AT+CIPSTART=\"TCP\",\"") + SERVER_IP + "\",8081\r\n").toCharArray(tcpHeader,50);
  Serial.write(tcpHeader);
  delay(1000);
  Serial.write("AT+CIPSEND=102\r\n"); // IMPORTAT: How long http request is? It must be properly set.
  delay(1000);
  char postHeader[100];
  (String("POST /air_temp=") + /*String(sensorsData[0],1)*/ air_temperature + "&air_humidity=" + air_humidity + "&luminocity=" + luminocity + "&soil_hum=" + soil_hum + " HTTP/1.1\r\n").toCharArray(postHeader,100);
  Serial.write(postHeader);
  Serial.write("Host: ");
  char postHost[50];
  (String(SERVER_IP) + "\r\n").toCharArray(postHost,50);
  Serial.write(postHost);
  Serial.println();
}

void checkIfPumpNeedToRun() {
  
}

// Turn on pump for specified time in miliseconds
void pumpWater(int timeToPump) {
  digitalWrite(WATERPUMP_DRIVER, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(timeToPump);                       // wait for a second
  digitalWrite(WATERPUMP_DRIVER, LOW);    // turn the LED off by making
}
// Read air temperature
float readAirTemp () {
  DHT.read11(dht_dpin);
  delay(200);
  return DHT.temperature;
  //float sensor_read_temp = 1;
  
  //return sensor_read_temp;
}
// Read air humidity
float readAirHum () {
  DHT.read11(dht_dpin);
  delay(200);
  return DHT.humidity;
  //float sensor_read_hum = 1;
  
  //return sensor_read_hum;
}
// Read soil humidity
float readSoilHum () {
  // read mockup
  float sensor_read_hum = 1;
  // real sensor read
  float soil_hum_val = analogRead(soil_hum);
  float soil_hum_val_p = map(soil_hum_val, soil_hum_max, soil_hum_min, 0, 100);

  return soil_hum_val_p;
  //return sensor_read_hum;
}
// Read light intensity
float readLightInt () {
  // read mockup
  float sensor_read_light = 1;
  // real sensor read
  float lum_val = analogRead(lum);
  delay(200);

  return lum_val;
  //return sensor_read_light;
}

