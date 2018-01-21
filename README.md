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
can read serial output. If you look at the code you'll see that I multiply the voltage reading by .977 for
no apparent reason. The reason is, the ADC isn't terribly accurate (there are standalone precision ADC chips
that cost more than an ESP8266 chip -- it's OK for the price but what do you expect from a built-in ADC on
a low-cost chip :-) Anyway, I saw a few posts on ESP8266 forums where people were measuring voltage with a
known-good voltmeter and adjusting their readings based on that -- the claim is that the ADC error is roughly
linear. In my case, multiplying the ESP8266's reading by .977 makes it very close to what I measure when I
connect a multimeter to the TMP36's output pin, so that's why I do it. Otherwise, the readings are consistently
high.

Next I ran the Web Client sample, almost unmodified. You have to enter your own SSID and Wi-Fi password to
make this work. If this one works, you're rocking.

Finally I used the MQTT_Watson sketch to write to the Watson IoT platform. The sketch for that also needs
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
2. I have a 350 mAH battery attached to my Feather. Starting from a full charge (4.20 V), it's been able to keep
   this sketch running for 3 hours (3.72 V after 3 hours). In that time, the re-connection logic in the sketch
   has not kicked in once -- the sketch has been happily running with its original connection the whole time.
   That is, the "connects" variable, which is incremented every time the sketch decides that it has to tear down
   and rebuild the connection to the MQTT broker, still has a value of 1.
3. As noted in the sketch, the blue LED will turn on when it's sending data to the MQTT broker (once every 15
   seconds), and the red LED lights up if it detects a connection problem and has to re-connect.

The MQTT_Watson LowPower sketch uses the "deep sleep" feature. It connects to MQTT, takes a temperature reading
and sends it to the MQTT broker, then goes to sleep for 2 minutes. When it wakes up, it starts over again. This
requires one additional jumper (connect GPIO #16 to RESET) but seems to reduce power consumption by about 90%.
It draws only about 8 mA while sleeping -- I think that's the serial chip which I don't think you can put to
sleep. I have a 350 mAH battery that can run the non-LowPower version of this sketch for about 3 hours; with the
LowPower version it looks like it will last about a day.
