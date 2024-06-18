#include <Wire.h>
#include "MAX30105.h" // Sparkfun MAX3010X library
#include <U8x8lib.h>
#include "heartRate.h"
#define BUZZERPIN 14

MAX30105 particleSensor;

const byte RATE_SIZE = 4; // Số chỉ số nhịp tim để tính trung bình
byte rates[RATE_SIZE]; // Mảng lưu chỉ số nhịp tim
byte rateSpot = 0;
long lastBeat = 0; // Thời điểm gần nhất phát hiện nhịp tim

float beatsPerMinute;
int beatAvg; // Chỉ số nhịp tim trung bình

// Mảng để lưu trữ giá trị LED của cảm biến
const int BUFFER_SIZE = 100;
long redBuffer[BUFFER_SIZE];
long irBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Khởi tạo màn hình OLED
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

void setup() {
  // Setup OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Setup buzzer
  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN,HIGH);

  // Setup sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use default I2C port, 400kHz speed
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    u8x8.drawString(0, 0, "MAX30105 not found");
    while (1);
  }
  u8x8.drawString(0, 0, "Place finger on");
  u8x8.drawString(0, 2, "sensor");

  particleSensor.setup(); // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
}
bool laststate = false; // Trạng thái đặt tay lên sensor gần nhất, = true nếu có đặt ngón tay lên sensor, = false nếu ngược lại
void loop() {
  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  if (irValue < 50000) {
    if(laststate == true) {
      beatAvg = 20;
      u8x8.clearDisplay();
      u8x8.drawString(0, 2, "No finger");
      digitalWrite(BUZZERPIN,HIGH);
    }
    laststate = false;
  } else {
    // Lưu giá trị vào buffer
    redBuffer[bufferIndex] = redValue;
    irBuffer[bufferIndex] = irValue;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

    // Nếu phát hiện nhịp tim thì lưu lại thời điểm này và tính chỉ số nhịp tim
    if (checkForBeat(irValue) == true) {
      // We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      // Nếu chỉ số nhịp tim hợp lệ thì lưu lại vào mảng và tính giá trị trung bình
      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE; // Wrap variable

        // Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
    
    // Tính toán SpO2
    float red_dc = calculateDC(redBuffer);
    float ir_dc = calculateDC(irBuffer);
    float red_ac = calculateAC(redBuffer);
    float ir_ac = calculateAC(irBuffer);

    float R = (red_ac / red_dc) / (ir_ac / ir_dc);
    float SpO2 = 123.7 - 25 * R;

    if(laststate == false) {
      u8x8.clearDisplay();
      u8x8.drawString(0, 2, "Avg BPM: ");
      u8x8.drawString(0, 4, "SpO2: ");
    }
      
      u8x8.setCursor(9, 2);
      u8x8.print(beatAvg);
      u8x8.setCursor(6, 4);
      if(SpO2 >= 100.00) SpO2 = 99.00;
      u8x8.print(SpO2);
      u8x8.print(" %");
      if(beatAvg >= 40 && beatAvg <= 100 && SpO2 >= 95) digitalWrite(BUZZERPIN,HIGH);
      else digitalWrite(BUZZERPIN,LOW);
    
    laststate = true;
  }

  delay(845);
}

float calculateDC(long* buffer) {
  float sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / BUFFER_SIZE;
}

float calculateAC(long* buffer) {
  long minValue = buffer[0];
  long maxValue = buffer[0];
  for (int i = 1; i < BUFFER_SIZE; i++) {
    if (buffer[i] < minValue) minValue = buffer[i];
    if (buffer[i] > maxValue) maxValue = buffer[i];
  }
  return maxValue - minValue;
}
