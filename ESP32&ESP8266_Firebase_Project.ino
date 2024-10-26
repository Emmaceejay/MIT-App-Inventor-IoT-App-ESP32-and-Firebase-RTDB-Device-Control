/*For this project I have used and ESP8266, for the code to work with an ESP32 you will have to uncomment the "#include <WiFi.h> 
and comment out the #include <ESP8266WiFi.h>*/
#include <ESP8266WiFi.h> //for ESP8266
//#include <WiFi.h>  //uncomment for ESP32
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Your-SSID"
#define WIFI_PASSWORD "SSID-PASSWORD"

/* 2. Define the API Key */
#define API_KEY "Your-Firebase-API"

/* 3. Define the RTDB URL */
#define DATABASE_URL "Your FBDB URL" 

/* 4. Define the user Email and password that are already registered or added in your project */
#define USER_EMAIL "DB USER NAME"
#define USER_PASSWORD "DB-PASSWORD"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

const int ledPin = 5;
const int ledPin2 = 4;
const int ledPin3 = 14;
const int gasPin = 27;

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);

  pinMode(ledPin3, OUTPUT);
  digitalWrite(ledPin3, LOW);

  pinMode(gasPin, INPUT);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the API key */
  config.api_key = API_KEY;

  /* Assign the user sign-in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the token generation task */
  config.token_status_callback = tokenStatusCallback; 

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Retrieve the string value from Firebase for LED1
    String ledState1;
    if (Firebase.RTDB.getString(&fbdo, "/house/led1", &ledState1))
    {
      if (ledState1 == "1")
        digitalWrite(ledPin, HIGH);  // Turn on LED1
      else if (ledState1 == "0")
        digitalWrite(ledPin, LOW);   // Turn off LED1
    }
    else
    {
      Serial.println(fbdo.errorReason().c_str());
    }

    // Retrieve the string value from Firebase for LED2
    String ledState2;
    if (Firebase.RTDB.getString(&fbdo, "/house/led2", &ledState2))
    {
      if (ledState2 == "1")
        digitalWrite(ledPin2, HIGH);  // Turn on LED2
      else if (ledState2 == "0")
        digitalWrite(ledPin2, LOW);   // Turn off LED2
    }
    else
    {
      Serial.println(fbdo.errorReason().c_str());
    }

    // Retrieve the string value from Firebase for LED3
    String ledState3;
    if (Firebase.RTDB.getString(&fbdo, "/house/led3", &ledState3))
    {
      if (ledState3 == "1")
        digitalWrite(ledPin3, HIGH);  // Turn on LED3
      else if (ledState3 == "0")
        digitalWrite(ledPin3, LOW);   // Turn off LED3
    }
    else
    {
      Serial.println(fbdo.errorReason().c_str());
    }

    // Sensor section
    Serial.printf("Gas Leak: %s\n", Firebase.RTDB.setBool(&fbdo, F("/house/gas-leak"), digitalRead(gasPin) == 0) ? "ok" : fbdo.errorReason().c_str());

    // Generate random temperature and humidity values
    float t = random(200, 350) / 10.0; // Random temperature between 20.0°C and 35.0°C
    float h = random(300, 900) / 10.0; // Random humidity between 30.0% and 90.0%

    Serial.printf("Temperature: %s\n", Firebase.RTDB.setFloat(&fbdo, F("/house/temp"), t) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Humidity: %s\n", Firebase.RTDB.setFloat(&fbdo, F("/house/humidity"), h) ? "ok" : fbdo.errorReason().c_str());
  }
}
