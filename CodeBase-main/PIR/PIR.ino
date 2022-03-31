int PIRledPin = 15;
int PIRpin = 9;
int val = 0;

void setup()
{
  pinMode(PIRledPin, OUTPUT);
  digitalWrite(PIRledPin, LOW);
  pinMode(PIRpin, INPUT);
}
void loop()
{
  val = digitalRead(PIRpin);
  digitalWrite(ledPin, val);

if (val ==1)
  digitalWrite(ledPin, LOW);
}
