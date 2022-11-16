#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ThingSpeak.h>
#include <PulseSensorPlayground.h>     //  Includes the PulseSensorPlayground Library

#define ONE_WIRE_BUS 2 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


PulseSensorPlayground pulseSensor;  // Creates an object

float bodytemperature;
int PulseSensorPurplePin = 0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED13 = 2;   //  The on-board Arduion LED
int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;            // Determine which Signal to "count as a beat", and which to ingore.
int myBPM;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClient client;

long myChannelNumber = 1678456;
const char myWriteAPIKey[] = "YQZWUN3ALU8NNZTG";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin("hitesh", "hitesh123");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  sensors.begin();

  pulseSensor.analogInput(PulseSensorPurplePin);   
  pulseSensor.setThreshold(Threshold);  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  ThingSpeak.begin(client);
}

void loop() {
  sensors.requestTemperatures(); 
  bodytemperature = sensors.getTempCByIndex(0);
  Signal = analogRead(PulseSensorPurplePin); 
  myBPM = Signal*0.125;
  display.clearDisplay();
  
  Serial.print("Body Temperature: ");
  Serial.print(bodytemperature);
  Serial.println("°C");
  Serial.print("BPM: ");
  Serial.println(myBPM); 

  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(" Body Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(bodytemperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("BPM: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(myBPM); 
  
  display.display(); 
  
  ThingSpeak.writeField(myChannelNumber, 1, bodytemperature , myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, myBPM , myWriteAPIKey);
  
  delay(10);
}
