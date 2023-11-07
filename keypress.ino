
#include "keypress.h"

static int  presstime_ms;

bool key_state()
{
  return(digitalRead(0));
}

void wait_for_key_release()
{
  do{
  }while(key_state() == PRESS);  //button released?
}

unsigned char process_keypress()
{
  char boot_key;
  static char old_boot_key;
  unsigned char Key_Event;

  Key_Event = INACTIVE;

  boot_key = key_state();

  //calculate press time
  if(boot_key == 0)
    presstime_ms += KEY_LOOP_TIME_MS;
  else 
    presstime_ms = 0; 

  if(boot_key != old_boot_key)
  {
    old_boot_key = boot_key;
    if(boot_key == 0) //pressed to low?
    {
      Key_Event = PRESS;
    }
    else
    {
      //release only if there was no short or long press before:
      Key_Event = RELEASE;
    }  
  }


  //check for short press
  if((presstime_ms > SHORT_TIME) && (presstime_ms < LONG_TIME))
    Key_Event = SHORT_PRESS;

  //check for long press
  if(presstime_ms > LONG_TIME)
     Key_Event = LONG_PRESS;

  //Serial.println(Key_Event);

  return(Key_Event);
}

int Get_presstime()
{
  return presstime_ms;
}