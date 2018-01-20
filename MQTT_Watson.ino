#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
 * LEDs both turn on at the start of setup, then off at the end (after first connect)
 * Blue LED turns on at the start of loop, then off near the end (before only the delay)
 * Red turns on when attempting to re-connect to broker, in case of connection failure
 */

#define TMP36_PIN          0 // Analog pin
#define RED_LED_PIN        0 // Digital pin
#define BLUE_LED_PIN       2 // Digital pin

#define TURN_ON            LOW
#define TURN_OFF           HIGH

#define MY_SSID            "*** YOUR SSID GOES HERE ***"
#define MY_WEP_PASSWORD    "*** YOUR WI-FI PASSWORD GOES HERE ***"

#define BROKER             "quickstart.messaging.internetofthings.ibmcloud.com"
#define TOPIC              "iot-2/evt/status/fmt/json"
#define MY_CLIENT_ID       "d:quickstart:ArduinoUnoWiFi:PDH-AU1"

/*
#define BROKER             "test.mosquitto.org"
#define TOPIC              "pdhArduinoTest1"
#define MY_CLIENT_ID       "pdh-MQTTpub-Win1-ID"
*/

char message[128];
#define DELAY_BT_READINGS  15 // Delay time in seconds between messages

WiFiClient client; int port = 1883;
PubSubClient clientPS(client);

int connects = 0;
long startTime, lastConnectTime;

void setup() {
  pinMode (RED_LED_PIN, OUTPUT);
  pinMode (BLUE_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, TURN_ON);
  digitalWrite (BLUE_LED_PIN, TURN_ON);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No WiFi shield");
    while (true);
  }
  
  clientPS.setServer(BROKER, port);
  
  startTime = millis();

  digitalWrite(RED_LED_PIN, TURN_OFF);
  digitalWrite (BLUE_LED_PIN, TURN_OFF);
}

void loop() {
  Serial.println ("Enter loop");
  digitalWrite (BLUE_LED_PIN, TURN_ON);
  
  while (!clientPS.connected()) {
    connect_to_MQTT();
  }

  int ar = analogRead(TMP36_PIN);
  // Serial.println (ar);
  double volts = ar / 1024.;
  // Serial.println (v, 3);
  volts *= .977; // Observed fudge factor
  float degreesC = (volts - 0.5) * 100.0;
  float degreesF = degreesC * (9.0/5.0) + 32.0;
  
  int rssi = WiFi.RSSI();
  long uptimeMinutes = (millis() - startTime) / (1000L * 60L);
  long connectUptime = (millis() - lastConnectTime) / (1000L * 60L);

  buildMessage (degreesF, volts, rssi, connects, uptimeMinutes, connectUptime);
  
  Serial.print ("Publish: "); Serial.println (message);
  Serial.print ("  Len: "); Serial.println (strlen(message)); // Max seems to be 96; MQTT_MAX_PACKET_SIZE
  clientPS.publish(TOPIC, message);

  delay(500);
  digitalWrite(BLUE_LED_PIN, TURN_OFF);

  Serial.println ("Loop: delay");
  /*
  for (int i = 0; i < DELAY_BT_READINGS; ++i) {
    delay (1000);
    if (i && i%20 == 0) {
      Serial.println ("Check connectivity...");
      boolean st = clientPS.loop();
      if (!st) {
        Serial.print("Lost connection, i = "); Serial.println (i);
        break;
      }
    }
  }
  */
  delay (DELAY_BT_READINGS * 1000);
  Serial.println ("Exit loop");
}

void connect_to_MQTT() {
  Serial.println ("Connect to MQTT");
  ++connects;

  connect_to_WiFi();

  digitalWrite(RED_LED_PIN, TURN_ON);
  
  if (!clientPS.connected()) {
    Serial.println("Connect to broker");
    boolean connected = clientPS.connect(MY_CLIENT_ID); 
    if (connected) {
      Serial.println("  Success");
    } else {
      Serial.print("  Failed, rc="); Serial.print(clientPS.state());
        Serial.println (" ; delay");
      delay(5000);
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

  lastConnectTime = millis();
  digitalWrite(RED_LED_PIN, TURN_OFF);
}

void connect_to_WiFi () {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connect to WiFi");
    WiFi.begin(MY_SSID, MY_WEP_PASSWORD);
    Serial.println("  Delay before use");
    delay(5000);
  }
}

void buildMessage(double temp, double volts, int rssi, int connects, long uptime, long connectUptime) {
  char tempBuf[5];
  
  strcpy (message, "{\"d\":{\"temp\":");
   dtostrf (temp, 5, 1, tempBuf);
   strcat (message, tempBuf);
  
  strcat (message, ", \"RSSI\":");
   itoa(rssi, tempBuf, 10);
   strcat (message, tempBuf);

  strcat (message, ", \"volt\":");
   dtostrf (volts, 4, 3, tempBuf);
   strcat (message, tempBuf);
  
  strcat (message, ", \"conns\":");
   itoa (connects, tempBuf, 10);
   strcat (message, tempBuf);
  
  strcat (message, ", \"uptime\":");
   ltoa (uptime, tempBuf, 10);
   strcat (message, tempBuf);
   
  strcat (message, ", \"connTime\":");
   ltoa(connectUptime, tempBuf, 10);
   strcat (message, tempBuf);

  strcat (message, "}}");
}
