
// *********************** use your blynk device credentials
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""
// ***********************************************

#include <ESP32Tone.h>
#include <WiFi.h>
#include "HTTPClient.h"
#include "time.h"
#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>
//BlynkTimer timer;

#define LDR_SENSOR_PIN 26
#define PIR_MOTION_SENSOR_PIN 27
#define SERVO_PIN 32
#define BUZZER_PIN 33
#define LED_RED_PIN 15
#define LED_WHITE_PIN 2
#define LED_GREEN_PIN 4

/*********** change these with appropriate values  *************/

const char* ssid = ""; // use your wifi network name
const char *password = ""; // use network password

String cnt = "node1"; // container i.e., "node" + node_number

String cse_ip = ""; // YOUR IP from ipconfig/ifconfig

char auth[] = BLYNK_AUTH_TOKEN;
/*********************************************************************/
String cse_port = "8080";
String onem2m_server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";


Servo servol;
const int EchoPin = 13;
const int TrigPin = 12;

float safeDistance = 100; // Distance is in SI Units

bool white_led_on = false;
bool red_led_on = false;
bool green_led_on = false;
int security_mode = 0;

float distance_to_onem2m;
int ldr_to_onem2m;
int pir_to_onem2m;



BLYNK_WRITE(V0)
{
  security_mode = param.asInt();
  if (security_mode)
  {
    digitalWrite(LED_GREEN_PIN, LOW);
  }
  else 
  {
    digitalWrite(LED_GREEN_PIN, HIGH);
  }
 // delay(5000);
}


void setup()
{
    Serial.println("Starting blynk");
  Blynk.begin(auth, ssid, password);
  Serial.println("Started Blynk");
  pir_to_onem2m = 0;
  ldr_to_onem2m = 0;
  distance_to_onem2m = 0;
  Serial.begin(115200);
  //   sensor pins setup
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  servol.attach(SERVO_PIN);
  Serial.print("Servo angle = ");
  Serial.println(servol.read());
  servol.write(0);
  delay(100);
  pinMode(PIR_MOTION_SENSOR_PIN, INPUT);

  // led pins
  pinMode(LED_WHITE_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_WHITE_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  //powerpins

  // network part
  Serial.println("Network Name : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.print("\n");
  Serial.println("WiFi connected..!");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());

  Serial.print("Wifi status = ");
  Serial.println(WiFi.status());

//timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  // network part
  //handle_network();

 Blynk.run();
//  timer.run(); 
  // main code part
  if (security_mode)
  {
    PIR_motion_sensing();

    createCI("PIR_motion_sensor", pir_to_onem2m);
  }
  else
  {
    Distance_sensing();
    LDR_sensing();
    createCI("LDR_sensor", ldr_to_onem2m);
    createCI("Ultrasonic_Distance_sensor", distance_to_onem2m);
  }
  //  Serial.println("sent data to onem2m "); // only data every 15sec intervel is saved to thins speak
 
  delay(1000);
  
}



//void myTimerEvent()
//{
//  
//  if (pir_to_onem2m == 1)
//  {
//    Serial.println("****  motion detected...Notifying the owner...*****\n");
//   
//  }
//}


void PIR_motion_sensing()
{
  // Turns on leds when light intensity is low.
  int pir_reading = digitalRead(PIR_MOTION_SENSOR_PIN);
  pir_to_onem2m = pir_reading;
  Serial.print("PIR reading = ");
  Serial.println(pir_reading);
  if (pir_reading == HIGH)
  {
    if (!red_led_on)
    {
      digitalWrite(LED_RED_PIN, HIGH);
      Serial.println("RED LED ON");
      tone(BUZZER_PIN, 1000);
      red_led_on = true;
    }
  }
  else
  {
    if (red_led_on)
    {
      digitalWrite(LED_RED_PIN, LOW);
      Serial.println("RED LED OFF");
      noTone(BUZZER_PIN);
      red_led_on = false;
    }
  }
}

void LDR_sensing()
{
  // Turns on leds when light intensity is low.
  int ldr_reading = digitalRead(LDR_SENSOR_PIN);
  ldr_to_onem2m = ldr_reading;
  Serial.print("LDR reading = ");
  Serial.println(ldr_reading);
  if (ldr_reading == HIGH)
  {
    if (!white_led_on)
    {
      digitalWrite(LED_WHITE_PIN, HIGH);
      Serial.println("WHITE LED ON");
      white_led_on = true;
    }
  }
  else
  {
    if (white_led_on)
    {
      digitalWrite(LED_WHITE_PIN, LOW);
      Serial.println("WHITE LED OFF");
      white_led_on = false;
    }
  }
}

float Get_Distance()
{
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  float time_lapse = pulseIn(EchoPin, HIGH);
  float distance = 0.017 * time_lapse;

  Serial.print("distance =  ");
  Serial.print(distance);
  Serial.print(" cm (");

  float distance_inches = distance * 0.393701;

  Serial.print(distance_inches);
  Serial.println(" inches )");
  return distance;
}

void move_servo_dial(float distance)
{
  // angle = 0.45*distance  where distance is in cm
  float angle = distance * 0.45;
  servol.write(angle);
  delay(100);
  Serial.print("Distance reading = ");
  Serial.print(distance);
  Serial.print("  angle = ");
  Serial.println(angle);
}

void Distance_sensing()
{
  float Distance = Get_Distance();
  distance_to_onem2m = Distance;
  Blynk.virtualWrite(1, distance_to_onem2m);
  move_servo_dial(Distance); //rotating servo motor

  if (Distance < safeDistance) //critical check
  {
    digitalWrite(LED_RED_PIN, HIGH);
    Serial.println("RED LED ON");
    tone(BUZZER_PIN, 1000);
    red_led_on = true;
  }
  else
  {
    digitalWrite(LED_RED_PIN, LOW);
    Serial.println("RED LED OFF");
    noTone(BUZZER_PIN);
    red_led_on = false;
  }
}

void createCI(String ae, float val)
{
  // creates a client instance
  HTTPClient http;
  http.begin(onem2m_server + ae + "/" + cnt + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}
