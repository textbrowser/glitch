// Generated by Glitch!
// Sat Jul 2 23:54:06 2022

int pushButton = 2;

void loop(void)
{
	int buttonState = input;
	Serial.println(buttonState);
	delay(1);
}

void setup(void)
{
	Serial.begin(9600);
}