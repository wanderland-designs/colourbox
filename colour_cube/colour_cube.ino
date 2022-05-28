/************************************************************************
  Name: Colour cube
  Author: A.Y.Gielen (1664336), CE Group 21
  Version: 1.0
  Date: April 10 2022
  Description: Colour cube is a teensy circuit to help colourblind children figure out the colour of their colour pencils.
  SPDX-FileCopyrightText: © 2022 Alice Gielen <a.y.gielen@student.tue.nl>
  SPDX-License-Identifier: Attribution-NonCommercial-ShareAlike 4.0 International
 ************************************************************************/

//Defining libraries
#include <EasyColor.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//LDR
int ldr = A0;
int value = 0;

//Color sensor
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define LightSensor A0
#define LED 15
String color = "No";

//RGB to HSV Converter
EasyColor::HSVRGB hsvConverter;
hsv out_hsv;
rgb in_rgb;

//SD CARD PIN DEFINING
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

//AUDIO
AudioPlaySdWav           playWav1;
AudioOutputAnalog        dac1;
AudioOutputPWM           pwm1;           //xy=331,125
AudioConnection          patchCord1(playWav1, 0, dac1, 0);
AudioConnection          patchCord2(playWav1, 0, pwm1, 0);
AudioControlSGTL5000     sgtl5000_1;

String audio_name = "No.WAV"; //random name, this is not an actual file

//SETUP
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(LightSensor, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void printValues()
{
  //Printing
  Serial.print("H=");
  Serial.print(out_hsv.h);
  Serial.print(", S=");
  Serial.print(out_hsv.s);
  Serial.print(", V=");
  Serial.print(out_hsv.v);
  Serial.print(color);
  Serial.println("  ");
  Serial.print("R=");
  Serial.print(in_rgb.r);
  Serial.print(", G=");
  Serial.print(in_rgb.g);
  Serial.print(", B=");
  Serial.print(in_rgb.b);
  Serial.println("  ");
}

void playAudio(String filename)
{
  Serial.print("Playing audio: ");
  Serial.println(filename);

  // Start playing file
  playWav1.play(filename.c_str());

  // A brief delay for the library read WAV info
  delay(5);

  // Waiting for the file to finish playing
  while (playWav1.isPlaying()) {

  }
}


//LOOP
void loop() {
  value = analogRead(ldr); //Reads the Value of LDR(light).
  //Serial.println("LDR value is :");//Prints the value of LDR to Serial Monitor.
  Serial.println(value);
  delay(100);
  if (value < 10)
  {
    Serial.println("sensor reading");
    digitalWrite(LED, HIGH); //LED on
    // Setting red filtered photodiodes to be read
    delay(500);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    // Reading the output frequency
    in_rgb.r = pulseIn(sensorOut, LOW);

    // Setting Green filtered photodiodes to be read
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    // Reading the output frequency
    in_rgb.g = pulseIn(sensorOut, LOW);

    // Setting Blue filtered photodiodes to be read
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    // Reading the output frequency
    in_rgb.b = pulseIn(sensorOut, LOW);

    // Converting RGB to HSV for easier categorization of colors
    out_hsv = hsvConverter.RGBtoHSV(in_rgb, out_hsv);

    //Determining color (color names are a subjective manner and are difficult to categorize into basic colors)
    if (out_hsv.v < 6 )   {
      color = "White";
      audio_name = "WHITE.WAV";
    } else if (out_hsv.v > 6 && out_hsv.s < 33) {
      color = "Black";
      audio_name = "BLACK.WAV";
    } else if (out_hsv.h < 20) {
      color = "Red";
      audio_name = "RED.WAV";
    } else if (out_hsv.h < 90) {
      color = "Yellow";
      audio_name = "YELLOW.WAV";
    } else if (out_hsv.h < 150) {
      color = "Green";
      audio_name = "GREEN.WAV";
    } else if (out_hsv.h < 270) {
      color = "Blue";
      audio_name = "BLUE.WAV";
    } else if (out_hsv.h < 330) {
      color = "Pink";
      audio_name = "PINK.WAV";
    }
    printValues();
    playAudio(audio_name);
    delay(2000);
  }
  else {
    digitalWrite(LED, LOW); //LED off
    Serial.print("LDR still on!");
    Serial.println("  ");
    delay(300);
  }
}
