#include "WiFi.h"
#include "ESPAsyncWebServer.h"

//pins
int tempPin = 32;
int motionSensor = 27;
//------------------------------------------

//variables for temperature sensor.
float temp;
float mV;
float cel;
bool flagFire = true;
//------------------------------------------

//variables for PIR sensor.
#define timeSeconds 1
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
bool flagInt = false;
//------------------------------------------

//variables for WiFi
AsyncWebServer server(80);
AsyncWebServerResponse *response;
AsyncWebServerRequest *request;
const char* ssid = "Government";
const char* password = "123456789";
int count = 0;
String sendM = "98";

//------------------------------------------

//Interrupt function for PIR
void IRAM_ATTR detectsMovement()
{
  //digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  //for PIR.
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
  //------------------------------------------

  //for WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", sendM);
    });
  server.begin();
  //------------------------------------------

}

void loop()
{
  // put your main code here, to run repeatedly:
  temp = analogRead(32);//TEMP
  delay(1000);
  mV = (temp * 3300) / 4096;
  cel = mV / 10;
  // Serial.print(String(temp) + " ");
  Serial.println(String(cel));

  if(flagFire)
  {
    //detecting human presence.
    now = millis();
    if(startTimer && (now - lastTrigger > (timeSeconds * 1000)))
    {
      Serial.println("Motion stopped...");
      //digitalWrite(led, LOW);
      startTimer = false;
    }

    //transmit data over WiFi.
    sendM = "0";
  }

  if(cel > 145)
    flagFire = true;
  //delay(1000);

}
