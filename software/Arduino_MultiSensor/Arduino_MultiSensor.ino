#include "Adafruit_SGP40.h"
#include "Adafruit_HTU21DF.h"
#include "Adafruit_BME680.h"
#include "SensirionI2CScd4x.h"
#include "Adafruit_TSL2591.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_SGP40 sgp;
Adafruit_HTU21DF htu;
Adafruit_BME680 bme;
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
SensirionI2CScd4x scd4x;
uint16_t error;
char errorMessage[256];

bool bsgp=true, bhtu=true, bbme=true, bscd = true, btsl = true; 

void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial console to open!

  Serial.println("room sensor test");

  if (!sgp.begin()){
    Serial.println("SGP4x sensor not found");
    bsgp = false;
  }

  if(bsgp) {
    Serial.print("SGP4x serial #");
    printSerialNumber(sgp.serialnumber[0], sgp.serialnumber[1], sgp.serialnumber[2]);
  }

  if (!htu.begin()) {   // Set to 0x45 for alternate i2c addr
    Serial.println("HTU21 sensor not found");
    bhtu = false;
  }

  if (!bme.begin()) {
    Serial.println("BME680 sensor not found");
    bbme = false;
  } else {
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
  }

  if(!tsl.begin()) {
    Serial.println(F("TSL2591 sensor not found"));
    btsl = false;
  } else {
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  }

  scd4x.begin(Wire);
  // stop potentially previously started measurement
  if (scd4x.stopPeriodicMeasurement()) {
      Serial.print("SCD4x: Error trying to execute stopPeriodicMeasurement(): ");
  }

  uint16_t serial0;
  uint16_t serial1;
  uint16_t serial2;
  if (scd4x.getSerialNumber(serial0, serial1, serial2)) {
    Serial.println("SCD4x sensor not found");
    bscd = false;
  } else {
    Serial.print("SCD4x serial #");
    printSerialNumber(serial0, serial1, serial2);
  }

  if(bscd) {
    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
  }

}

void loop() {
  uint16_t sraw;
  int32_t voc_index;
  float t=25, h=50, p, alt;
  uint16_t co2 = 0;
  bool isDataReady = false;
  
  if(bhtu) {
    Serial.print("HTU21:\t");
    t = htu.readTemperature();
    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
    h = htu.readHumidity();
    Serial.print("Hum. % = "); Serial.println(h);
    //Serial.println();
  }

  if(bbme) {
    if(bme.performReading()) {
      Serial.print("BME68X:\t");
      t = bme.temperature;
      Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
      h = bme.humidity;
      Serial.print("Hum. % = "); Serial.print(h); Serial.print("\t\t");
      p = bme.pressure / 100.0;
      Serial.print("Press. hPa = "); Serial.print(p); Serial.print("\t\t");
      alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
      Serial.print("Approx. Altitude m = "); Serial.println(alt);
      //Serial.println();
    }
  }

  if(btsl) {
    Serial.print("TSL2561:");
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    Serial.print("IR: "); Serial.print(ir); Serial.print("\t\t");
    Serial.print("Full: "); Serial.print(full);  Serial.print("\t\t");
    Serial.print("Visible: "); Serial.print(full - ir);  Serial.print("\t\t\t");
    Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir), 6);
    //Serial.println();
  }

  if(bsgp) {
    Serial.print("SGP4x:\t");
    sraw = sgp.measureRaw(t, h);
    Serial.print("Raw measurement: "); Serial.print(sraw);  Serial.print("\t");

    voc_index = sgp.measureVocIndex(t, h);
    Serial.print("Voc Index: "); Serial.println(voc_index); 
    //Serial.println();
  }

  if(bscd) {
    scd4x.getDataReadyFlag(isDataReady);
    if(isDataReady) {
      Serial.print("SCD4x:\t");
      if (scd4x.readMeasurement(co2, t, h)) {
        Serial.print("Error trying to execute readMeasurement(): ");
      } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
      } else {
        Serial.print("Co2 ppm = "); Serial.println(co2);
        //Serial.println();
      }
    }
  }

  Serial.println("==================================================================\n");


  delay(1000);
}
