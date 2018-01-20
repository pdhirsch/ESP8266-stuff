// Connections are: VCC --- OUT --- GND
// A 0.22 uF cap (224) across pins 1 and 3 wouldn't hurt

#define TMP36_PIN   0 // Analog pin 0
#define DELAY       5 // Seconds

void setup()
{
  Serial.begin(9600);
  Serial.println ("TMP36 temperature sensor test");
}

void loop()
{
  int ar = analogRead(TMP36_PIN); // ar will be in [0, 1023]
  
  float voltage = ar / 1024.;
  voltage *= .977; // Observed fudge factor
  float degreesC = (voltage - 0.5) * 100.0;
  float degreesF = degreesC * (9.0/5.0) + 32.0;

  Serial.print("Analog reading: "); Serial.print (ar);
  Serial.print("; voltage = "); Serial.print(voltage, 3);
  Serial.print("; temp (F) = "); Serial.println(degreesF, 1);
  
  delay(DELAY * 1000);
}
