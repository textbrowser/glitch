// Generated by Glitch!
// Wed Oct 19 21:07:15 2022

void loop(void)
{
 int sensorValue  = analogRead(A0);
 Serial.println(sensorValue);
 delay(1);
}

void setup(void)
{
 Serial.begin(9600);
}
