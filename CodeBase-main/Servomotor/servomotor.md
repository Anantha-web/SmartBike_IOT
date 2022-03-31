# Servomotor

![Servomotor Connections](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQHD1oynUUXupA7551FZ7i2uJJFn8ghwM1xPA&usqp=CAU)


- As in above diagram, brown wire is connected to `GND` , red wire to `3.3 V` and orange wire to Control pin
- [This library](https://github.com/RoboticsBrno/ESP32-Arduino-Servo-Library/archive/master.zip) can be used to control servomotor
- To test the working of servomotor use the following code
- Servomotor works fine, using GPIO 32, as proposed by Narayana.

```
#include <Servo.h>

static const int servoPin = 32;

Servo servo1;

void setup() {
    Serial.begin(9600);
    servo1.attach(servoPin);
}

void loop() {
    for(int posDegrees = 0; posDegrees <= 180; posDegrees+=60) {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(1000);
    }

    for(int posDegrees = 180; posDegrees >= 0; posDegrees-=60) {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(1000);
    }
}

```

* To dictate the angle of servomotor, use the `servo1.write()` method.
* In `setup()` block, use `servo.attach()` method, to initialise.
