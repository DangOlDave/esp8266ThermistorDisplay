

//thermistortemp_disp
//This is an elaboration of thermistortemp, with the addition of a 128x64 I2C display
//I worked out taking 10 voltage samples from
//a thermistor voltage divider and averaging and converting to a temperature
//using the equation for resistance of a 3435B thermistor. This is for an
//ESP8266 NodeMCU board. The display is an OLED I2C 128x64 that uses the SS1306 controller.
//I am using Adafruit libraries for displaying data.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1

#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ADC_PIN = A0; //Pin on NodeMCU board for analog to digital input

// Thermistor parameters
const float BETA = 3435.0;
const float R0   = 10000.0;   // 10k @ 25C
const float T0   = 298.15;    // 25C in kelvin
const float VCC  = 3.3;

// ADC parameters
const float ADC_MAX = 1023.0;

// Averaging parameters
const int   NUM_SAMPLES = 10;
const int   SAMPLE_DELAY_MS = 100;  // 10 samples over 2 seconds

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);


}

void loop() {
  long adc_sum = 0;

  // Take 10 samples over ~2 seconds
  for (int i = 0; i < NUM_SAMPLES; i++) {
    adc_sum += analogRead(ADC_PIN);
    delay(SAMPLE_DELAY_MS);
  }

  float adc_avg = adc_sum / (float)NUM_SAMPLES;

  // Convert averaged ADC to voltage
  float Vout = VCC * (adc_avg / ADC_MAX);

  // Basic sanity check
  if (Vout <= 0.001 || Vout >= VCC) {
    Serial.println("ADC out of range");
    return;
  }

  // Thermistor resistance (high-side thermistor)
  float Rtherm = R0 * (VCC / Vout - 1.0);

  // Temperature calculation (B-parameter)
  float invT  = (1.0 / T0) + (1.0 / BETA) * log(Rtherm / R0);
  float tempC = (1.0 / invT) - 273.15;

  //Serial.print("ADC avg: "); 
  //Serial.print(adc_avg, 1);
  Serial.print("  Temp: ");
  Serial.print(tempC, 2);
  Serial.println(" C");
  display.clearDisplay();
  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0); // Start at top-left corner
  display.println(F("OSM Overtemp Sensor"));
  display.println("Temperature");
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(tempC,2);
  display.print(" ");
  display.print((char)247);
  display.println("C");
  display.display();
}
