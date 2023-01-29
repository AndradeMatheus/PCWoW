#include "credentials.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "frontpanel-utils.h"
#include "connectivity-utils.h"

//Set-time to update
const int UPDATE_POWER_STATE = 15; // Change to how many seconds you want it to push to the MQTT topic
int CYCLE = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);

	while (WiFi.begin(NET_SSID, NET_PASS) != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(5000);
  }
	Serial.println("Connected to WiFi.");

	while (!client.connected()) {
		Serial.println("Connecting to MQTT...");
 
    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)){
      Serial.println("Connected to MQTT.");
    }
  	else {
      Serial.println((String)"Connection to MQTT failed: " + client.state());
      delay(2000);
    }
  }

  while(!getTopic(MQTT_TOPIC_BUTTON))
    delay(1000);
}

void loop()
{
	delay(1000);
	client.loop();

	bool powerState = getPowerState();

	if(CYCLE == UPDATE_POWER_STATE){
		CYCLE = 0;
		bool mqttClientResponse = pushTopic(MQTT_TOPIC_STATUS, String(powerState).c_str());
		Serial.println(mqttClientResponse ? "Topic published." : "Error on publish.");
	}
	else CYCLE++;
}
