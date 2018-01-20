ESP8266-stuff: various sketches for use with ESP8266 boards (tested with Adafruit Feather Huzzah) and the Arduino IDE

I followed the setup instructions at:
  https://learn.adafruit.com/adafruit-feather-huzzah-esp8266?view=all

You have to install a serial/USB driver, and add the ESP8266 board to your Arduino IDE. After those one-time
startup steps, working with the ESB8266 is very similar to working with an Arduino.
The only problem I ran into was that at first I was using a power-only microUSB cable (without knowing it).
Those are commonly used to recharge batteries and they look identical to data-capable cables from the
outside, but they don't work to upload sketches. Make sure you have a data-capable USB cable!

I started with the Blink sketch, as usual. Note that the ESP8266 I/O pins are active low, not active high
as on the Arduino, so a LOW digital write turns an LED on, and HIGH turns it off.

Then I added a TMP36 temperature sensor; the TMP36 sketch shows that you can do analog input, and that you
can read serial output.

Next I ran the Web Client sample, almost unmodified. You have to enter your own SSID and Wi-Fi password to
make this work. If this one works, you're rocking.

Finally I used the MQTT_Watson script to write to the Watson IoT platform. The sketch for that also needs
your SSID and password. There are commented-out lines that allow you to test with mosquitto.org rather than
Watson if you'd like to do that. A few other random notes about this sketch:
0. There's some Watson IoT setup necessary to make this work -- specifically you have to create the device
   that's mentioned in the CLIENT_ID, and you will therefore have to change the CLIENT_ID to match your own
   device identifier. After doing that, you can see your data via a URL like this (also has the device ID in it):
   https://quickstart.internetofthings.ibmcloud.com/#/device/PDH-AU1/sensor/
   You can do this without creating a paid account on IBM Cloud -- no credit card or other payment needed.
1. The WiFi.begin() call is a bit different from the Arduino version if you use WEP (as I do). With the "standard"
   Arduino WiFi library you have to pass in an additional argument, your WEP key number. That's not needed with
   this WEP library -- WEP and WPA work the same, just two arguments in both cases.
2. During the delay between sending data readings to the MQTT broker, I used to call PubSubClient.loop() in
   the Arduino version of this sketch (used with the ATWINC1500 Wi-Fi chip), because that seemed to help keep
   the connection alive in between data transmissions. With the ESP8266 on the other hand, that seemed to make
   it much less stable, which is why that code is commented out. I need to look into this in more detail.
3. I have a 350 mAH battery attached to my Feather. Starting from a full charge (4.20 V), it's been able to keep
   this sketch running for 3 hours (3.71 V after 3 hours). In that time, the re-connection logic in the sketch
   has not kicked in once -- the sketch has been happily running with its original connection the whole time.
   That is, the "connects" variable, which is incremented every time the sketch decides that it has to tear down
   and rebuild the connection to the MQTT broker, still has a value of 1.
4. As noted in the sketch, the blue LED will turn on when it's sending data to the MQTT broker (once every 15
   seconds), and the red LED lights up if it detects a connection problem and has to re-connect.
