#include "menue.h"
#include "keypress.h"

static unsigned char menue_size = 1;
static unsigned char Selection;


void clear_cursor(unsigned char pos)
{
  //clear old cursor
  gfx->fillRect(0 , MENUE_START_Y + pos * LINE_DISTANCE ,19 ,MENUE_START_Y + (pos + 1) * LINE_DISTANCE,WHITE);
}

//void Draw_Menue(char* text[], unsigned char lines)
void Draw_Menue(str_menue menue)
{
  unsigned char i;
  Selection = 0;
  menue_size = menue.menue_lines;
  
  //start menue only if key is released..
  wait_for_key_release();

  //clear area complete:
  gfx->fillRect(0,MENUE_START_Y,MENUE_END_X,MENUE_END_Y,WHITE);

  //clear old headline
  gfx->fillRect(0,MENUE_HEADLINE_Y,MENUE_END_X,MENUE_HEADLINE_Y + 10, WHITE);

  //print the headline
  gfx->setCursor(20, MENUE_HEADLINE_Y);  
  gfx->println(menue.headline);  

  //draw the Text
  for(i=0;i < menue.menue_lines; i++)
  {
    gfx->setCursor(20, MENUE_START_Y + i * LINE_DISTANCE);  
    gfx->println(menue.items[i]);
  }
  
}

unsigned char process_cursor(unsigned char key_event)
{
    unsigned int  i;

    //set new cursor:
    if(key_event == RELEASE)
    {
      //clear old cursor
      clear_cursor(Selection);

      //increment menue
      Selection++;
      Selection = Selection % menue_size;

      //draw new cursor
      gfx->setCursor(MENUE_START_X, MENUE_START_Y + Selection * LINE_DISTANCE);
      gfx->print(">");     
    }   

    //set countdown cursor
    if(key_event == SHORT_PRESS)
    {
      clear_cursor(Selection);
      gfx->setCursor(MENUE_START_X, MENUE_START_Y + Selection * LINE_DISTANCE);
      i = LONG_TIME - Get_presstime();  //remaining time
      
      //i = i /1000;  //in sec
      //gfx->print(i);
      i = (i/100) % 8;
      gfx->print(rotate[i]);
            
    }

  return Selection;

}
