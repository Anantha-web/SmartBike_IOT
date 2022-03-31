#include <ESP32Tone.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <ESP32Servo.h>

#define LDR_SENSOR_PIN 26
#define PIR_MOTION_SENSOR_PIN 27
#define SERVO_PIN 32
#define BUZZER_PIN 33
#define LED_RED_PIN 15
#define LED_WHITE_PIN 2
#define LED_GREEN_PIN 4

/*********** change these with appropriate values  *************/

const char* ssid = "network name"; // use your wifi network name
const char *password = "password"; // use network password

String cnt = "node1"; // container i.e., "node" + node_number

String cse_ip = "127.0.0.1"; // YOUR IP from ipconfig/ifconfig
/*********************************************************************/
String cse_port = "8080";
String onem2m_server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";

WiFiServer server(80);

Servo servol;
const int EchoPin = 13;
const int TrigPin = 12;

float safeDistance = 100; // Distance is in SI Units

bool white_led_on = false;
bool red_led_on = false;
bool green_led_on = false;
bool security_mode = false;

float distance_to_onem2m;
int ldr_to_onem2m;
int pir_to_onem2m;

void setup()
{
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

  server.begin();
  Serial.println("Server started!!!");
  Serial.print("Wifi status = ");
  Serial.println(WiFi.status());

}

void loop()
{
  // network part
  handle_network();

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

void handle_network()
{
  WiFiClient client = server.available();

  if (client)
  {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("content-type:text/html");
            client.println();
            client.print("<h1> Security mode </h1>");
            client.print(" <span style=\"background-color: rgb(0, 255, 0)\; border-radius: 10px\;padding: 10px\;font-size: 30px\;\" ><a href=\"H\" style=\"text-decoration: none\;\"> ON</a> </span><br><br><br><br>");
            client.print(" <span style=\"background-color: rgb(255, 0, 0)\; border-radius: 10px\;padding: 10px\;font-size: 30px\;\" ><a href=\"L\" style=\"text-decoration: none\;\"> OFF </a> </span><br>");
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /L"))
        {
          security_mode = false;
          digitalWrite(LED_RED_PIN, LOW);
          digitalWrite(LED_WHITE_PIN, LOW);
          digitalWrite(LED_GREEN_PIN, HIGH);
          noTone(BUZZER_PIN);
          Serial.println("Security mode Turned off");
        }
        else if (currentLine.endsWith("GET /H"))
        {
          security_mode = true;
          digitalWrite(LED_RED_PIN, LOW);
          digitalWrite(LED_WHITE_PIN, LOW);
          digitalWrite(LED_GREEN_PIN, LOW);
          noTone(BUZZER_PIN);
          delay(5000);
          Serial.println("Security mode Turned on");
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void PIR_motion_sensing()
{
  // Turns on leds when light intensity is low.
  int pir_reading = digitalRead(PIR_MOTION_SENSOR_PIN);
  pir_to_onem2m=pir_reading;
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
  ldr_to_onem2m=ldr_reading;
  Serial.print("LDR reading = ");
  Serial.println(ldr_reading);
  if ( ldr_reading == HIGH)
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
  move_servo_dial(Distance);  //rotating servo motor

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


void createCI(String ae, float val) {
  // creates a client instance
  HTTPClient http;
  http.begin(onem2m_server + ae + "/" + cnt + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}
