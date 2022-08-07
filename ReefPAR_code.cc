#include <ESP8266WiFi.h>
#include <inttypes.h>
#include <AH_24Cxx.h>
#include <Wire.h> 
#include <lm75.h>

#define AT24C08  3
#define BUSADDRESS  0x50      
#define EEPROMSIZE  1024     //AT24C08 1024 byte

WiFiServer server(80);

AH_24Cxx Ext_EEPROM  = AH_24Cxx(AT24C08, BUSADDRESS);
TempI2C_LM75 T_Sense = TempI2C_LM75(0x48,TempI2C_LM75::nine_bits);

const char WiFiAPPSK[] = "123456789"; //wifi password should be 8 characters long.

const int  PLink_InPin = A0;   
const int   Fan_OutPin = 16;  //       Fan output pin
const int  Blue_OutPin =  4;  //  Blue LED output pin
const int    UV_OutPin =  5;  //    UV LED output pin
const int Green_OutPin = 12;  // Green LED output pin 
const int White_OutPin = 13;  // White LED output pin
const int   Red_OutPin = 15;  //   Red LED output pin

     byte addr_11 = 11;
     byte addr_12 = 12;
     byte addr_13 = 13;
     byte addr_14 = 14;
     byte addr_15 = 15;   

int  Blue_Pow_Get =  0;
int    UV_Pow_Get =  0;
int Green_Pow_Get =  0;
int White_Pow_Get =  0;
int   Red_Pow_Get =  0;

int  Blue_Pow_Set =  0;
int    UV_Pow_Set =  0;
int Green_Pow_Set =  0;
int White_Pow_Set =  0;
int   Red_Pow_Set =  0;

int  PLink_InVal  =  0;  // value read from PowerLink Port
int  PLink_OutVal =  0;  // value to output of a LED pin

int temp;
unsigned long previousMillis = 0;

void setup() 
{    
  initHardware(); 
  setupWiFi();     
  server.begin();  
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {     
    unsigned long currentMillis = millis(); 
    if (currentMillis - previousMillis >= 5000) 
    { 
       Serial.begin(115200);
       previousMillis = currentMillis;         
       temp = T_Sense.getTemp();
       Serial.print("Temp: ");
       Serial.print(temp); 
       Serial.println("°C");    
    }
    if (temp < 30)
    {
       digitalWrite(Fan_OutPin, LOW); 
    }
    if (temp > 35)
    {
       digitalWrite(Fan_OutPin, HIGH); 
    }
    if (temp < 45)
    {
       analogWrite( Blue_OutPin, Blue_Pow_Set);
       analogWrite(   UV_OutPin,   UV_Pow_Set);
       analogWrite(Green_OutPin,Green_Pow_Set);
       analogWrite(White_OutPin,White_Pow_Set);
       analogWrite(  Red_OutPin,  Red_Pow_Set); 
    }  
    if (temp > 45)
    {
       analogWrite( Blue_OutPin, Blue_Pow_Set/20);
       analogWrite(   UV_OutPin,   UV_Pow_Set/20);
       analogWrite(Green_OutPin,Green_Pow_Set/20);
       analogWrite(White_OutPin,White_Pow_Set/20);
       analogWrite(  Red_OutPin,  Red_Pow_Set/20); 
    } 
    return;
  }  
  /*
  PLink_InVal  = analogRead(PLink_InPin);  
  PLink_OutVal = map(PLink_InVal, 0, 1023, 0, 255);
  
  // print the readings in the Serial Monitor
  Serial.print("PLink_InVal = ");
  Serial.print(PLink_InVal);
  Serial.print("\t PLink_OutVal = ");
  Serial.println(PLink_OutVal);
  */
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  // Match the request
  int val = -1; 
                
  if (req.indexOf("/save") != -1)
  {
      Serial.print("Saved");
      Ext_EEPROM.write_byte(addr_11, Blue_Pow_Get);
      delay(50);
      Ext_EEPROM.write_byte(addr_12,   UV_Pow_Get);
      delay(50);
      Ext_EEPROM.write_byte(addr_13,Green_Pow_Get);
      delay(50);
      Ext_EEPROM.write_byte(addr_14,White_Pow_Get); 
      delay(50);   
      Ext_EEPROM.write_byte(addr_15,  Red_Pow_Get); 
      delay(50);
  }
 
  for(int pos = 1020; pos >= 0; pos--)
  {  
      String slider1     = "/slider_blue/";   // Slider for Blue LED
      String slider_pos1 = slider1 + pos;       
      String slider2     = "/slider_uv/";     // Slider for UV LED 
      String slider_pos2 = slider2 + pos;    
      String slider3     = "/slider_green/";  // Slider for Green LED
      String slider_pos3 = slider3 + pos;  
      String slider4     = "/slider_white/";  // Slider for White LED
      String slider_pos4 = slider4 + pos;   
      String slider5     = "/slider_red/";    // Slider for Red LED
      String slider_pos5 = slider5 + pos;   
   
      if (req.indexOf(slider_pos1) != -1)
      {     
          analogWrite(Blue_OutPin, pos);
          Serial.print("Blue ");
          Serial.println(slider_pos1);
          Blue_Pow_Get = pos/4;
          Blue_Pow_Set = pos;
          pos = 0;
          break;
      }
      if (req.indexOf(slider_pos2) != -1)
      {     
          analogWrite(UV_OutPin, pos);
          Serial.print("UV ");
          Serial.println(slider_pos2);
          UV_Pow_Get = pos/4;
          UV_Pow_Set = pos;
          pos = 0;
          break;
      }
      if (req.indexOf(slider_pos3) != -1)
      {     
          analogWrite(Green_OutPin, pos);
          Serial.print("Green ");
          Serial.println(slider_pos3);         
          Green_Pow_Get = pos/4;
          Green_Pow_Set = pos;
          pos = 0;
          break;
      }
      if (req.indexOf(slider_pos4) != -1)
      {     
          analogWrite(White_OutPin, pos);
          Serial.print("White ");
          Serial.println(slider_pos4);
          White_Pow_Get = pos/4;
          White_Pow_Set = pos;
          pos = 0;
          break;
      }
      if (req.indexOf(slider_pos5) != -1)
      {     
          analogWrite(Red_OutPin, pos);
          Serial.print("Red ");
          Serial.println(slider_pos5);
          Red_Pow_Get = pos/4;
          Red_Pow_Set = pos;
          pos = 0;
          break;
      }   
  }  
  client.flush(); 
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ReefPAR " + macID;
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);
  for (int i=0; i<AP_NameString.length(); i++)
  AP_NameChar[i] = AP_NameString.charAt(i);
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  delay(1000);
  Wire.begin(2,14);
  Serial.begin(115200);
  
  Blue_Pow_Set  = Ext_EEPROM.read_byte(addr_11) * 4;
  UV_Pow_Set    = Ext_EEPROM.read_byte(addr_12) * 4;
  Green_Pow_Set = Ext_EEPROM.read_byte(addr_13) * 4;
  White_Pow_Set = Ext_EEPROM.read_byte(addr_14) * 4;
  Red_Pow_Set   = Ext_EEPROM.read_byte(addr_15) * 4;

  pinMode( PLink_InPin,  INPUT);
  
  pinMode( Blue_OutPin, OUTPUT);
  pinMode(   UV_OutPin, OUTPUT);
  pinMode(Green_OutPin, OUTPUT);
  pinMode(White_OutPin, OUTPUT);
  pinMode(  Red_OutPin, OUTPUT);  
  pinMode(  Fan_OutPin, OUTPUT); 
  
  digitalWrite(Fan_OutPin, LOW); 
  
  temp = T_Sense.getTemp();
  Serial.println(temp);
}


