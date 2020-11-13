#include <WiFi.h>
#include <EEPROM.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define AIO_SERVER      "io.adafruit.com"

#define AIO_SERVERPORT  1883                  

#define AIO_USERNAME    "Chandru421"

#define AIO_KEY         "aio_PWDZ95XLRpJLMPt0HR1VVo09JqXd"

#define RXD2 16
#define TXD2 17


const char* ssid ="virus";
const char* pass="Calciumzeta@4388";


WiFiClient client;     // Create an ESP8266 WiFiClient class to connect to the MQTT server.

void MQTT_connect();

 
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);        // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light_on");
Adafruit_MQTT_Subscribe dim = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light_on");

void setup() 
{

  Serial.begin(115200);

  delay(10);

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));

 // Connect to WiFi access point.

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  Serial.println();

 Serial.println("WiFi connected");

  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&light_on);


}

uint32_t x=0;

void loop()
{
  
     if(WiFi.status() != WL_CONNECTED) 
     {
       
       WiFi.begin(ssid,pass);

      while (WiFi.status() != WL_CONNECTED)
      {
  
        delay(500);
      
       Serial.print(".");

      }

      Serial.println();

      Serial.println("WiFi connected");

      Serial.println("IP address: "); Serial.println(WiFi.localIP());


     }

   MQTT_connect();

   Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(5000)))
  {

    if(subscription == &light)
    {

      Serial.print(F("Got: "));

      Serial.println((char *)light.lastread);

      if (!strcmp((char*) light.lastread, "ON"))
      {

        Serial.print("Channel 1 ON");
        Serial2.print('A');

      }
      else
      {
          Serial.print("Channel 1 OFF");
          Serial2.print('B');      
        

      }

    }
    if(subscription == &light_intensity)
    {
      Serial.print(F("Got: "));

      Serial.println((char *)dim.lastread);
      if (!strcmp((char*) light.lastread, "dim"))
      {

        Serial.print("Channel 1 DIM");
        Serial2.print('C');

      }
    }
    

}

}

void MQTT_connect() {

  int8_t ret;

  // Stop if already connected.

  if (mqtt.connected()) {

    return;

  }

 Serial.print("Connecting to MQTT... ");

uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected

       Serial.println(mqtt.connectErrorString(ret));

       Serial.println("Retrying MQTT connection in 5 seconds...");

       mqtt.disconnect();

       delay(5000);  // wait 5 seconds

       retries--;

       if (retries == 0) {

         // basically die and wait for WDT to reset me

         while (1);

       }

  }

  Serial.println("MQTT Connected!");

}
