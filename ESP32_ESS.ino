/*
Example Code To Get ESP32 To Connect To A Router Using WPS
===========================================================
This example code provides both Push Button method and Pin
based WPS entry to get your ESP connected to your WiFi router.

Hardware Requirements
========================
ESP32 and a Router having WPS functionality

This code is under Public Domain License.

Author:
Pranav Cherukupalli <cherukupallip@gmail.com>
*/

#include <EEPROM.h>
#include <stdarg.h>

#include "WiFi.h"
#include "esp_wps.h"

//Display
#include <Arduino_GFX_Library.h>
#include "img_logo.h"

//Key
#include "keypress.h"

//Menue
#include "menue.h"

#define GFX_BL DF_GFX_BL  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define TFT_BL 14

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
Arduino_DataBus *bus = new Arduino_ESP32SPI(11 /* DC */, 10 /* CS */, 12 /* SCK */, 13 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 1 /* RST */, 1 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */, 0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);


/*
Change the definition of the WPS mode
from WPS_TYPE_PBC to WPS_TYPE_PIN in
the case that you are using pin type
WPS
*/
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

static esp_wps_config_t config;
bool connected = false;         //eth connection

//IP
IPAddress my_ip;
IPAddress serverIP;
uint16_t serverPort = 502;   //Modbus-TCP
WiFiClient client;

//Menue
unsigned char Keypress;
unsigned char Menue_selection;

//Backlight
unsigned char Backlight = 100;

void BacklightMenue()
{
  float BL_percent;
  //Draw_Menue(BacklightText,BL_MENUE_LINES);
  Draw_Menue(MBacklight);
  do
  {
    delay(KEY_LOOP_TIME_MS);
    Keypress = process_keypress();
    Menue_selection = process_cursor(Keypress);
    //check for execution:
    if(Keypress >= SHORT_PRESS)
    {
      switch(Menue_selection)
      {
        case 0: if(Backlight<255) 
                  Backlight++; 
                break;
        case 1: if(Backlight > 1)
                  Backlight--; 
                break;
        case 2: if(Keypress == LONG_PRESS)
                  return;        
      }
    }
    ledcWrite(0, Backlight);

  gfx->fillRect(200,MENUE_HEADLINE_Y,MENUE_END_X,MENUE_HEADLINE_Y + 10, WHITE);
  gfx->setCursor(200, MENUE_HEADLINE_Y);  
  BL_percent = Backlight / 2.55;
  gfx->print(String(BL_percent)+" %");  

  }while(1);
}
  

void StartMenue()
{
  //Draw_Menue(StartMenueText,START_MENUE_LINES);
  Draw_Menue(MStartMenue);
  do
  {
    delay(KEY_LOOP_TIME_MS);
    Keypress = process_keypress();
    Menue_selection = process_cursor(Keypress);

    //check for execution:
    if(Keypress == LONG_PRESS)
    {
      switch(Menue_selection)
      {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: BacklightMenue(); break;
        case 4: return; break;
      }
    }
 
  }while(1);
}

void wpsInitConfig(){
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}

void wpsStart(){
    if(esp_wifi_wps_enable(&config)){
    	gfx->println("WPS Enable Failed");
    } else if(esp_wifi_wps_start(0)){
    	gfx->println("WPS Start Failed");
    }
}

void wpsStop(){
    if(esp_wifi_wps_disable()){
    	gfx->println("WPS Disable Failed");
    }
}

String wpspin2string(uint8_t a[]){
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}


unsigned char serach_mobus_server()
{
  unsigned char ip3;

  //client.setConnectionTimeout(100);
  for(ip3=1;ip3<255;ip3++)
  {
    serverIP[3]=ip3;
    gfx->fillRect(0,60,319,70,WHITE);
    gfx->setCursor(0, 60);
    gfx->println(serverIP); 

    //connect to server
    if (client.connect(serverIP, serverPort))
    {
      return 1;
    }      
  }
  return 0;
}

/*
void xprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Serial.println(format,args);
    va_end(args);
} 
*/


void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
  switch(event){
    case ARDUINO_EVENT_WIFI_STA_START:
      gfx->println("Station Mode Started");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      gfx->println("Connected to :" + String(WiFi.SSID()));
      gfx->println("Got IP: ");
      my_ip = WiFi.localIP();
      gfx->println(my_ip);
      connected=true;
      serverIP = my_ip;  
      if(serach_mobus_server() == true)
      {
        gfx->println("server found at "+ serverIP);
      } 
      else
      {
        gfx->println("no Server found!");
      }   
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      gfx->println("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      gfx->println("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      gfx->println("WPS Failed, retrying");
      wpsStop();
      wpsStart();
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      gfx->println("WPS Timedout, retrying");
      wpsStop();
      wpsStart();
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      gfx->println("WPS_PIN = " + wpspin2string(info.wps_er_pin.pin_code));
      break;
    default:
      break;
  }
}

void setup()
{

  int i;

  EEPROM.begin(1024);

  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  //Button
  pinMode(0, INPUT); //Pin D0 to input - boot Taster


  //init the display
  gfx->begin();
  gfx->fillScreen(WHITE);
  
  //Backlight
  #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    ledcSetup(0, 600, 8);
    ledcAttachPin(TFT_BL, 0);
    ledcWrite(0, Backlight); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

  #endif

  //display logo
  gfx->draw16bitRGBBitmap(10, 10, (uint16_t *)gImage_img_logo, 240, 46);

  //check Key
  gfx->setTextColor(BLACK);

  i=10;
  do
  {
    i--;
    delay(500);
    gfx->setCursor(MENUE_START_X, MENUE_START_Y);
    gfx->fillRect(MENUE_START_X,MENUE_START_Y,319,MENUE_START_Y + 10,WHITE);    
    gfx->print("Press BOOT to enter startup menue..");
    gfx->println(i);
    if(digitalRead(0) == 0 )  //button pressed to low?
    {
      StartMenue(); //Enter the Start menue
      i=0;
    }  
  }while(i);


  //start WPS
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);
  gfx->println("Starting WPS");
  wpsInitConfig();
  wpsStart();
}

bool OLD_D0;
void loop(){
  if(digitalRead(0) != OLD_D0)
  {
    OLD_D0 = digitalRead(0);
    Serial.println(OLD_D0);
  }
  //nothing to do here
}
