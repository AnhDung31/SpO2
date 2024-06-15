#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h" //sparkfun MAX3010X library
#include <U8x8lib.h>
#include "heartRate.h"   

#define OLED_SCK_PIN 5
#define OLED_SDA_PIN 4
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, OLED_SCK_PIN, OLED_SDA_PIN);
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8X8LOG u8x8log;
#define BUZZERPIN 14

MAX30105 particleSensor;
///////////////////////////////////////////////////////////////////////////////////
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
long lastprint = 0;
long lastNoFinger = 0;
long startFingerOn = 0;
bool fingeron = false;
double oxi;
long lastBuzz = 0;

double avered = 0; double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
int i = 0;
int Num = 100;//calculate SpO2 by this sampling interval
 
double ESpO2 = 93.0;//initial value of estimated SpO2
double FSpO2 = 0.7; //filter factor for estimated SpO2
double frate = 0.95; //low pass filter for IR/red LED value to eliminate AC component
#define TIMETOBOOT 2000 // wait for this time(msec) to output SpO2
#define SAMPLING 1 //if you want to see heart beat more precisely , set SAMPLING to 1
#define MINIMUM_SPO2 0.0


void output() {
  u8x8log.print("\f");
  u8x8log.printf("HR: %d bpm\n",beatAvg);
  u8x8log.printf("SpO2: %.1lf %%\n",oxi);
  if(fingeron && oxi < 95) digitalWrite(BUZZERPIN, LOW);
  else digitalWrite(BUZZERPIN, HIGH);
}
void setup()
{
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(0);	// 0: Update screen with newline, 1: Update screen for every char
  u8x8log.print("Initializing..\n");
  delay(1000);

  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN,HIGH);
  Serial.begin(115200);
 
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    u8x8log.print("MAX30102 was not found. Please check wiring/power and restart.\n");
    while (1);
  }
 
  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 70; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  // Set up the wanted parameters
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

void loop()
{
 
  uint32_t ir, red;
  double fred, fir;
  double SpO2 = 0; //raw SpO2 before low pass filtered
 
  particleSensor.check(); //Check the sensor, read up to 3 samples
 
  while (particleSensor.available()) {//do we have new data
      long timeStartCalcSPO2 = millis();
      red = particleSensor.getFIFORed(); //Sparkfun's MAX30105
      ir = particleSensor.getFIFOIR();  //Sparkfun's MAX30105
      i++;
      fred = (double)red;
      fir = (double)ir;
      avered = avered * frate + (double)red * (1.0 - frate);//average red level by low pass filter
      aveir = aveir * frate + (double)ir * (1.0 - frate); //average IR level by low pass filter
      sumredrms += (fred - avered) * (fred - avered); //square sum of alternate component of red level
      sumirrms += (fir - aveir) * (fir - aveir);//square sum of alternate component of IR level    

      if ((i % SAMPLING) == 0) {//slow down graph plotting speed for arduino Serial plotter by thin out
        if (ir < 50000) ESpO2 = MINIMUM_SPO2; //indicator for finger detached
        if (ESpO2 <= -1) ESpO2 = 0;
        if (ESpO2 > 100) ESpO2 = 100;

        oxi = ESpO2;
        
        //Serial.print(" Oxygen % = ");
        //Serial.println(ESpO2); //low pass filtered SpO2
      }
      if ((i % Num) == 0) {
        double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
        // Serial.println(R);
        SpO2 = -23.3 * (R - 0.4) + 100; //http://ww1.microchip.com/downloads/jp/AppNotes/00001525B_JP.pdf
        ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2;//low pass filter
        //  Serial.print(SpO2);Serial.print(",");Serial.println(ESpO2);
        //Serial.print(" Oxygen2 % = ");
        //Serial.println(ESpO2); //low pass filtered SpO2
        sumredrms = 0.0; sumirrms = 0.0; i = 0;
        if(millis() - startFingerOn > 5000 && fingeron) output();
        break;
      }
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
      //Serial.println(SpO2);

       //////////////////////////////////////////////////////////////////////////////
      long irHR = particleSensor.getIR();
      if(irHR >= 50000){
        if(fingeron == false) {
          startFingerOn = millis(); 
          u8x8log.print("\f");
          u8x8log.println("Please keep your");
          u8x8log.println("finger on sensor");
          u8x8log.println("for at least");
          u8x8log.println("5 secs.");
          fingeron = true;
        }             

        if (checkForBeat(irHR)) {    //If a heart beat is detected
          long timeDetectHeartBeat = millis();
          long delta = timeDetectHeartBeat - lastBeat - (timeDetectHeartBeat - timeStartCalcSPO2);                   //Measure duration between two beats
          
          beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM
      
          if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To calculate the average we strore some values (4) then do some math to calculate the average
          {
            rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
            rateSpot %= RATE_SIZE; //Wrap variable
      
            //Take average of readings
            beatAvg = 0;
            for (byte x = 0 ; x < RATE_SIZE ; x++)
              beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
          }

          lastBeat = millis();
        }
        // Serial.print("IR=");
        // Serial.print(irHR);
        // Serial.print(", BPM=");
        // Serial.print(beatsPerMinute);
        // Serial.print(", Avg BPM=");
        // Serial.println(beatAvg);
      }
      else {       //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
        beatAvg=0; 
        fingeron = false;        
        if(millis() - lastNoFinger > 1000) {
          lastNoFinger = millis();
          u8x8log.print("\f");
          u8x8log.println("No finger");
        }
        digitalWrite(BUZZERPIN,HIGH);
      }
    
  } 
}

