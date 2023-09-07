#include <WiFi.h>
#include <LoRa.h> 
#include <Firebase_ESP_Client.h>
#include "time.h"
// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert your network credentials
#define WIFI_SSID "Orange-0F20"
#define WIFI_PASSWORD "RR804L649GD"
#define API_KEY "AIzaSyD3YIlyZqdDEQac-dpXroK6nOsTLF_pT70"
#define DATABASE_URL "https://esp32test-4f2c1-default-rtdb.europe-west1.firebasedatabase.app/" 
#define USER_EMAIL "zorgatitayeb5@gmail.com"
#define USER_PASSWORD "18/08/2023"
#define NSS 5
#define RST 14
#define DI0 2
// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String databasePath;
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/Acceleration";
String vitPath = "/vitesse";
String timePath = "/timestamp";
String parentPath;
int timestamp;
FirebaseJson json;
const char* ntpServer = "pool.ntp.org";
String Acceleration;
String temperature;      // This string hold the temperature data
String humidity;
String vitesse ; 
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 2000;
String lora_data = "";
int pos_1, pos_2, pos_3,pos_4 ; 
String  counter ;
// int compteur ;
// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup(){
  Serial.begin(115200);
   //Serial.begin(115200);

  //setup LoRa sender
  LoRa.setPins(NSS, RST, DI0);

  //866E6 for Europe
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
  
  // Change sync word (0xF0) to match the receiver

  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa init succeeded.");
  // Initialize BME280 sensor
  
  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  databasePath =  "/SensorsData/";
  // compteur= 0 ; 
}

void loop(){
     // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
  
    String lora_data = LoRa.readString();

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    // print RSSI of packet

    pos_1 = lora_data.indexOf('a');
    pos_2 = lora_data.indexOf('b');
    pos_3 = lora_data.indexOf('c');
    pos_4 = lora_data.indexOf('d');
    

    temperature = lora_data.substring(0, pos_1);
    humidity = lora_data.substring(pos_1 + 1, pos_2);
    Acceleration = lora_data.substring(pos_2 + 1,pos_3);
    vitesse = lora_data.substring(pos_3 + 1,pos_4 );
    counter = lora_data.substring(pos_4+ 1, lora_data.length());
    Serial.print("Received packet:  ");
    Serial.println(counter);

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Acceleration: ");
    Serial.println(Acceleration);
    Serial.print("vitesse:");
    Serial.println(vitesse);
    Serial.print("with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.println("----------------");}
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);
    // compteur ++ ; 
    parentPath= databasePath ;
   
    //json.set(tempPath.c_str(), String(bme.readTemperature()));
    json.set(tempPath.c_str(),String(temperature.toFloat()));
    json.set(humPath.c_str(),String(humidity.toFloat()));
    json.set(presPath.c_str(),String(Acceleration.toFloat()));
    json.set(vitPath.c_str(),String(vitesse.toFloat()));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

}
