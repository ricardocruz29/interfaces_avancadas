#include "SoftwareSerial.h"
#include "DFPlayerMini_Fast.h"
#include <Adafruit_NeoPixel.h>

//DFPLAYER
const int RX_DF_PIN = 9;
const int TX_DF_PIN = 8;
SoftwareSerial dfSerial(RX_DF_PIN, TX_DF_PIN);
DFPlayerMini_Fast player;

//RGB LED STRIP
#define PIN            3
#define NUMPIXELS      9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Movement Sensor
const int movementPin = 4;
int movementValCurrent = LOW; // current state of movement
int movementValPrevious  = LOW; // previous state of movement

//Soil Humidity Sensor
const int humidityPin = 1;
float humidityPercentage = 0;
float totalHumidityPercentage = 0;
bool badHumidity = false;

//Temperature Sensor
const int temperaturePin = 2;
float temperatureCelsius = 0;
float totalTemperatureCelsius = 0;
bool badTemperature = false;

//Luminosity Sensor
const int ldrPin = 0;
float lux = 0;  //ldrVal converted to lux
float totalLux = 0;
float adcValue = 0.0048828125; //constant used in converting to lux
bool badLdr = false;

//measurements
int nrMeasures = 0;
const int measurement_interval = 600; // -> in real life, we would measure 5 in 5 minutes. In 24 hours, it would lead to 288 measurements. Considering 3 minutes instead of 24hours, and maintaining 288 measurements, that leads to a measurement every 0.6s
bool allMeasuresOk = false;
bool prevAllMeasuresOk = false;

//audio tracks
const int goodMorning = 1;
const int allOk = 15;
const int excessiveLight = 3;
const int missingLight = 5;
const int missingWater = 11;
const int excessiveWater = 13;
const int excessiveHeat = 7;
const int missingHeat = 9;

void setup() {
  pinMode(movementPin, INPUT);
  pinMode(humidityPin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ldrPin, INPUT);

  // Initialize Pixels
  pixels.begin();

  //Initialize Df Player
  dfSerial.begin(9600);
  player.begin(dfSerial);
  player.volume(100);

  // Initialize Console
  Serial.begin(9600);
}

void loop() {
  movementValPrevious = movementValCurrent; //store old state of movement
  movementValCurrent = digitalRead(movementPin); //read new state of movement
  prevAllMeasuresOk = allMeasuresOk; //Store previous measurements value

  Serial.println(movementValCurrent);
  Serial.println(movementValPrevious);

  if (movementValPrevious == LOW && movementValCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Alguém entrou na sala!");
    Serial.println(analogRead(humidityPin));

    //Turn leds On when someone enters
    pixels.setBrightness(100);
    pixels.show();

    //ldr
    lux = getAverage(totalLux);
    //Serial.println(lux);
    //temperature
    temperatureCelsius = getAverage(totalTemperatureCelsius);
    //Serial.println(temperatureCelsius);
    //humidity
    humidityPercentage = getAverage(totalHumidityPercentage);
    //Serial.println(humidityPercentage);

    checkMeasures(true);
  }
  else if (movementValPrevious == HIGH && movementValCurrent == HIGH) {
    Serial.println("Alguém ainda se está a mexer");
    Serial.println(analogRead(humidityPin));

    if (!allMeasuresOk) {
      //Recheck humidity while person in the room
      humidityPercentage = getHumidityConvertedToPercentage(analogRead(humidityPin));
      //Serial.println(humidityPercentage);

      //Recheck ldr while person in the room
      lux = getLdrConvertedToLux(analogRead(ldrPin));
      //Serial.println(lux);

      //Recheck temperature while person in the room
      temperatureCelsius = getTemperatureConvertedToCelsius(analogRead(temperaturePin));
      //Serial.println(temperatureCelsius);

      checkMeasures(false);
    }
  }
  else if (movementValPrevious == LOW && movementValCurrent == LOW) {  // pin state change: HIGH -> LOW
    Serial.println("Não está ninguém na sala!");
    pixels.setBrightness(1);
    pixels.show();

    totalLux += getLdrConvertedToLux(analogRead(ldrPin));
    Serial.println(getLdrConvertedToLux(analogRead(ldrPin)));
    totalTemperatureCelsius += getTemperatureConvertedToCelsius(analogRead(temperaturePin));
    Serial.println(getTemperatureConvertedToCelsius(analogRead(temperaturePin)));
    totalHumidityPercentage += getHumidityConvertedToPercentage(analogRead(humidityPin));
    Serial.println(getHumidityConvertedToPercentage(analogRead(humidityPin)));

    nrMeasures += 1;
    delay(measurement_interval);
  }
}

//Converts analogic read of ldr to Lux
float getLdrConvertedToLux(int ldrVal) {
  return (250.000000 / (adcValue * ldrVal)) - 50.000000;
}

//Converts analogic read of temperature to celsius
float getTemperatureConvertedToCelsius(int temperatureVal) {
  return temperatureVal * 0.0048828125 * 100;
}

//Convertes analogic read of humidity to Percentage
float getHumidityConvertedToPercentage(int humidityVal) {
  //humidityVals come between 10 and 75, being 10 dry (0%) and 75 at max humidity (100%)
  //for calc reasons, we assume values are between 0 and 65, and when we receive humidityVal we subtract 10 also.
  return ((humidityVal - 10) * 100) / 65;
}

//returns the average of a certain measure
float getAverage(float total) {
  return total / nrMeasures;
}

//Function that puts the led on a certain color
void setLedColor(int R, int G, int B) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
  }
}

//Function that fades in
void fadeIn() {
  for (int brightness = 1; brightness <= 100; brightness = brightness + 3) {
    pixels.setBrightness(brightness);
    pixels.show();
    delay(20);
  }
}

//Function that fades out
void fadeOut() {
  for (int brightness = 100; brightness >= 1; brightness = brightness - 3) {
    pixels.setBrightness(brightness);
    pixels.show();
    delay(20);
  }
}

//function that checks all measures, and do the consequences
//If the person just entered the room, it will play the audio and activate leds
//If the person is still in the room, it will just activate leds, and when every measure is ok, it activates an audio saying everything ok.
void checkMeasures(bool firstMovementDetected) {
  //Check Humidity
  if (humidityPercentage < 40) { //Dry
    Serial.println("Dry");
    badHumidity = true;
    setLedColor(0, 255, 255);
    fadeIn();
    if (firstMovementDetected) {
      player.play(missingWater);
    }
    delay(3000);
    fadeOut();
  }
  else if (humidityPercentage > 70) { //Moist
    Serial.println("Moist");
    badHumidity = true;
    setLedColor(0, 0, 255);
    fadeIn();
    if (firstMovementDetected) {
      player.play(excessiveWater);
    }
    delay(3000);
    fadeOut();
  } else {
    Serial.println("Perfect Humidity");
    badHumidity = false;
  }

  //Check Brightness
  if (lux < 90) { //Dark
    Serial.println("Dark");
    badLdr = true;
    setLedColor(100, 25, 200);
    fadeIn();
    if (firstMovementDetected) {
      player.play(missingLight);
    }
    delay(3000);
    fadeOut();
  }
  else if (lux > 1000) { //Bright
    Serial.println("Bright");
    badLdr = true;
    setLedColor(255, 255, 255);
    fadeIn();
    if (firstMovementDetected) {
      player.play(excessiveLight);
    }
    delay(3000);
    fadeOut();
  } else {
    Serial.println("Perfect Luminosity");
    badLdr = false;
  }

  //Check Temperature
  if (temperatureCelsius < 18) { //Cold
    Serial.println("Cold");
    badTemperature = true;
    setLedColor(60, 0, 255);
    fadeIn();
    if (firstMovementDetected) {
      player.play(missingHeat);
    }
    delay(3000);
    fadeOut();
  }
  else if (temperatureCelsius > 25) { //Hot
    Serial.println("Hot");
    badTemperature = true;
    setLedColor(255, 0, 0);
    fadeIn();
    if (firstMovementDetected) {
      player.play(excessiveHeat);
    }
    delay(3000);
    fadeOut();
  } else {
    Serial.println("Perfect Temperature");
    badTemperature = false;
  }

  //If everything is ok then play a sound saying everything is ok
  if (!badLdr && !badTemperature && !badHumidity) {
    Serial.println("All OK!");
    fadeIn();
    setLedColor(255, 150, 0);
    allMeasuresOk = true;
    if (firstMovementDetected) { //If the person first entered the room, it will play goodMorning audio
      player.play(goodMorning);
    } else {
      if (!prevAllMeasuresOk) { //It it wasn't everything Ok, and now it is, play audio.
        player.play(allOk);
      }
    }
    delay(3000);
    fadeOut();
  } else {
    allMeasuresOk = false;
  }
}
