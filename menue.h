
#ifndef MENUE_H
#define MENUE_H

//Startup Menue Text
#define MENUE_START_X     10
#define MENUE_END_X       310
#define MENUE_HEADLINE_Y  70
#define MENUE_START_Y     90
#define MENUE_END_Y       160
#define LINE_DISTANCE     10

typedef struct str_menue
{
  char*         headline;
  unsigned char menue_lines;
  char*         items[5];
  str_menue*    prev_menue;
}_MITEM;

str_menue MStartMenue = {
                          "START MENUE",
                          5,
                          { "0 RESET local IP Address", "1 RESET Cerbo IP Address", "2 FACTORY RESET","3 BACKLIGHT","4 EXIT AND REBOOT"},
                          0
                        };

str_menue MBacklight = {
                          "BACKLIGHT",
                          3,
                          {"0 BRIGHTER","1 DARKER","2 EXIT","",""},
                          &MStartMenue
                        };

//#define START_MENUE_LINES 5 
//char* StartMenueText[START_MENUE_LINES]={ "0 RESET local IP Address", "1 RESET Cerbo IP Address", "2 FACTORY RESET","3 BACKLIGHT","4 EXIT AND REBOOT"};

//#define BL_MENUE_LINES 3 
//char* BacklightText[BL_MENUE_LINES] ={"0 BRIGHTER","1 DARKER","2 EXIT"};

void Draw_Menue(char* text[], unsigned char lines);
unsigned char process_cursor(unsigned char key_event);

#endif