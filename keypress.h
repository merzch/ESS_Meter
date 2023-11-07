
#ifndef KEYPRESS_H
#define KEYPRESS_H

#define KEY_LOOP_TIME_MS     100

#define  INACTIVE    0
#define  PRESS       1
#define  RELEASE     2
#define  SHORT_PRESS 3
#define  LONG_PRESS  4 


//KEY Press Konfiguration
#define  SHORT_TIME  500  //ms
#define  LONG_TIME   3000 //ms

char rotate[8]={"-\|/-\|/"};

bool key_state();
void wait_for_key_release();
unsigned char process_keypress();  
int Get_presstime();

#endif