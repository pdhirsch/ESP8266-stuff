#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define TMP36_PIN          0 // Analog pin
#define RED_LED_PIN        0 // Digital pin
#define BLUE_LED_PIN       2 // Digital pin

#define TURN_ON            LOW
#define TURN_OFF           HIGH

#define MY_SSID            "*** YOUR SSID HERE ***"
#define MY_WEP_PASSWORD    "*** YOUR WI-FI PASSWORD HERE ***"

#define BROKER             "quickstart.messaging.internetofthings.ibmcloud.com"
#define TOPIC              "iot-2/evt/status/fmt/json"
#define MY_CLIENT_ID       "d:quickstart:ArduinoUnoWiFi:PDH-AU1"

/*
#define BROKER             "test.mosquitto.org"
#define TOPIC              "pdhArduinoTest1"
#define MY_CLIENT_ID       "pdh-MQTTpub-Win1-ID"
*/

char message[128];
#define DELAY_BT_READINGS  120 // Delay time in seconds between messages

WiFiClient client; int port = 1883;
PubSubClient clientPS(client);

void setup() {
  pinMode (RED_LED_PIN, OUTPUT);
  pinMode (BLUE_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, TURN_ON);
  digitalWrite (BLUE_LED_PIN, TURN_ON);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  Serial.println ("In setup...");

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No WiFi capability");
    while (true);
  }
  Serial.println ("WiFi OK");
  
  clientPS.setServer(BROKER, port);

  doWork();

  Serial.println ("Sleep...");
  ESP.deepSleep(DELAY_BT_READINGS * 1000000, WAKE_RF_DEFAULT);
}

void doWork() {
  Serial.println ("Enter doWork");
  
  while (!clientPS.connected()) {
    connect_to_MQTT();
  }

  digitalWrite(RED_LED_PIN, TURN_OFF);

  int ar = analogRead(TMP36_PIN);
  // Serial.println (ar);
  double volts = ar / 1024.;
  // Serial.println (v, 3);
  volts *= .977; // Observed fudge factor
  float degreesC = (volts - 0.5) * 100.0;
  float degreesF = degreesC * (9.0/5.0) + 32.0;
  
  int rssi = WiFi.RSSI();

  buildMessage (degreesF, volts, rssi);
  
  Serial.print ("Publish: "); Serial.println (message);
  Serial.print ("  Len: "); Serial.println (strlen(message)); // Max seems to be 96; MQTT_MAX_PACKET_SIZE
  clientPS.publish(TOPIC, message);

  delay(500);
  digitalWrite(BLUE_LED_PIN, TURN_OFF);
  Serial.println ("Exit doWork");
}

void connect_to_MQTT() {
  Serial.println ("Connect to MQTT");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connect to WiFi");
    WiFi.begin(MY_SSID, MY_WEP_PASSWORD);
    Serial.println("  Delay before use");
    digitalWrite(RED_LED_PIN, TURN_OFF);
    delay(4000);
    digitalWrite(RED_LED_PIN, TURN_ON);
  }
  
  if (!clientPS.connected()) {
    Serial.println("Connect to broker");
    boolean connected = clientPS.connect(MY_CLIENT_ID); 
    if (connected) {
      Serial.println("  Success");
    } else {
      Serial.print("  Failed, rc="); Serial.print(clientPS.state());
        Serial.println (" ; delay");
      digitalWrite(RED_LED_PIN, TURN_OFF);
      delay(500);
      digitalWrite(RED_LED_PIN, TURN_ON);
      /*
       * State codes:
       * -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
       * -3 : MQTT_CONNECTION_LOST - the network connection was broken
       * -2 : MQTT_CONNECT_FAILED - the network connection failed
       * -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
       *  0 : MQTT_CONNECTED - the cient is connected
       *  1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
       *  2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
       *  3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
       *  4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
       *  5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
       */
    }
  }
}

void buildMessage(double temp, double volts, int rssi) {
  char tempBuf[5];
  
  strcpy (message, "{\"d\":{\"Temp\":");
   dtostrf (temp, 5, 1, tempBuf);
   strcat (message, tempBuf);
  
  strcat (message, ", \"RSSI\":");
   itoa(rssi, tempBuf, 10);
   strcat (message, tempBuf);

  strcat (message, ", \"volt\":");
   dtostrf (volts, 4, 3, tempBuf);
   strcat (message, tempBuf);

  strcat (message, "}}");
}

void loop() { } // Never called
