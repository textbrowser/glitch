// Generated by Glitch!
// Wed Oct 19 21:08:23 2022

int ReadSens_and_Condition(void)
{
 int i ;
 int sval  = 0;
 sval = ((sval) / (5));
 for(i = 0; i < 5; i++)
 {
  sval = ((sval) + (analogRead(0)));
 }
 sval = ((sval) / (4));
 sval = ((255) - (sval));
 return(sval);
}

void loop(void)
{
}

void setup(void)
{
}
