const int EchoPin = 13;
const int TrigPin = 12;
Servo servol;
void setup()
{
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(9600);  
  servo1.attach(servoPin);
}

void loop()
{
  float A = GiveDistance();
  Serial.println(A);
  distansoMeter(A);
  delay(1000);
}

/* 
 *  This function returns the distance at the time when it is called.
 *  It returns a float value. The units are centimeter.
 *
 *  EchoPin and TrigPin global variables are used.
 *  https://create.arduino.cc/projecthub/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-036380  
 *  https://www.tutorialspoint.com/arduino/arduino_ultrasonic_sensor.htm
 */
float GiveDistance()
{
  float Duration, Distance;
  
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  Duration = pulseIn(EchoPin, HIGH);
  Distance = (Duration*.0343)/2;
  delay(100); 

  return Distance;
}

/*
 In this implementation if indicator shows  `0`, then the distance is a large number(or danger is `0`)
 and if indicator shows a large angle then there is more danger, i.e distance is almost zero.
*/

void distansoMeter(float distance)
{
    float safeDistance = 10; // Distance is in SI Units
    float angle = (safeDistance - distance)*120/safeDistance;
    servol.write(angle);

    delay(100);
}