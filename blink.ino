// Just make the red LED blink
// Note that LOW turns it on and HIGH turns it off (opposite of Arduino)

#define LED_PIN 0

#define ON_MS      3000
#define OFF_MS     1000

#define TURN_ON    LOW
#define TURN_OFF   HIGH

void setup() {
  pinMode (LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, TURN_ON);
  delay(ON_MS);
  digitalWrite(LED_PIN, TURN_OFF);
  delay(OFF_MS);
}
