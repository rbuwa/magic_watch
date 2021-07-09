/*designed by R.I BUWANEKA
 MAGIC_WATCH VERSION_1
 BASED ON MAGICBLOCK.IO
 BOARD ESP 32
*/
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <WiFiUdp.h>

static const unsigned char PROGMEM  mail[] =
{ 
B01111111, B11111110, 
B11000000, B00000011, 
B10110000, B00001101, 
B10001100, B00110001, 
B10000011, B11000001, 
B10000000, B00000001, 
B10000000, B00000001, 
B01111111, B11111110
};

static const unsigned char PROGMEM  LUX[] =
{ 
B00000000, B00000000, 
B00000001, B10000000, 
B00100001, B10000000, 
B00010001, B10001000, 
B00001001, B10010000, 
B00000100, B00100000, 
B00000001, B10000000, 
B01111011, B11011110,

B11110011, B11011110, 
B00000001, B10000000, 
B00000100, B00100000, 
B00001001, B10010000, 
B00010001, B10001000, 
B00000001, B10000100, 
B00000001, B10000000, 
B00000000, B00000000
};

const char* ssid = "SLT";
const char* password = "RT995997";

const char* mqtt_server = "broker.hivemq.com";

#define NTP_OFFSET  19800 
#define NTP_INTERVAL 60 * 1000  
#define NTP_ADDRESS  "lk.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
char daysOfTheWeek[7][12] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THUESDAY", "FRIDAY", "SATURDAY"};
WiFiClient espClient;
PubSubClient client(espClient);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
long lastMsg = 0;
char msg[50];
int value = 0;

int H,M ;
int n =0;
String lux = "0";



void setup(){
Serial.begin(9600);
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
display.setTextColor(BLACK,WHITE);
display.setTextSize(2);
display.setCursor(8, 20);
display.print("MAGICWATCH");//logo
display.setTextSize(0.5);
display.setCursor(38, 40);
display.setTextColor(WHITE);
display.println("Version_1");
display.display(); 
delay(3000);

  wifi_setup();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  timeClient.begin();
  
}

///////////////////////////////wifi///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void wifi_setup(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
   
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
   

  }
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////callback///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


 void callback(char* topic, byte* message, unsigned int length) {
  char str[length+1];
  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("]");
   String messageTemp;
   int i=0;
   for (i=0;i<length;i++) {
   //Serial.print((char)message[i]);
   str[i]=(char)message[i];
   messageTemp += (char)message[i];}
   
   str[i] = 0; // Null termination
   
   
     Serial.println();
    StaticJsonDocument <256> doc;
    deserializeJson(doc,str);

  String note = doc["Message"] ;
  Serial.print(note);
  String  lux = doc["lightSensor"]["illuminance"];
   H =  timeClient.getHours();
   M =  timeClient.getMinutes();  
  String formattedTime = timeClient.getFormattedTime();
  display.clearDisplay();
 
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(20, 20);

  if(H < 10){
    String hi = String(M);
    String nH = "0"+hi;
    display.print(nH);
   }
   else{
  display.print(H);}
  display.print(":");
  
  if(M < 10){
    String mi = String(M);
    String nM = "0"+mi;
    display.print(nM);
   }
   else{
  display.print(M);}
  
  display.setTextSize(1);
  display.setCursor(4,4);
  display.print(daysOfTheWeek[timeClient.getDay()]);
  display.drawBitmap(80, 4,  mail, 16, 8, 1); 
  display.setTextSize(1);
   
   if (note == "noti"){
      n = n + 1 ;
       if (n>10){
         n = 0;}}

  display.setCursor(100, 4);
  display.print("x");
  display.setCursor(110, 4);
  display.print(n);

 


  display.drawBitmap(45, 45,  LUX, 16, 16, 1); 
       display.setTextSize(2);
       display.setCursor(65, 45);
       if(lux == "null"){
        display.print("0");}
        else{
       display.print(lux);
        }
  
  display.display();  
  delay(100);

  //if (String(topic) == "2147sub") {
    //Serial.println("Temperature is ");
    //Serial.println(temp);
    
 
 }
/////////////////////////////////////////////////////////////////////////////////

////////////////////////////////reconnect////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("2147sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      
    }
  }
}
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void state(){
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
      
}

///////////////////////////////////////////////////////////////////////////








void loop()
{
  
  timeClient.update();
  state();
  
}
