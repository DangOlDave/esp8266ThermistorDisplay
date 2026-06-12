

//thermistortemp
//This program is where I worked out taking 10 voltage samples from
//a thermistor voltage divider and averaging and converting to a temperature
//using the equation for resistance of a 3435B thermistor. This is for an
//ESP8266 NodeMCU board.

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
}
