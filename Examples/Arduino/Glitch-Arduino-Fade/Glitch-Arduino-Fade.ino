// Generated by Glitch!
// Mon Apr 21 19:59:19 2025

int brightness  = 0;

int fadeAmount  = 5;

int led  = 9;

void loop(void)
{
 analogWrite(led, brightness);
 brightness = ((brightness) + (fadeAmount));
 if(brightness <=0 || brightness >= 255)
  {
   fadeAmount = ((0) - (fadeAmount));
  }
 delay(30);
}

void setup(void)
{
 pinMode(led, OUTPUT);
}
