/*
Name
  USB Multimedia Controller
Purpose
  Using Arduino ATMega32u4 5v, aka Pro Micro, take input from rotary encoder with switch and mimic a USB HID Keyboard to send the commands to the connected computer (PC or Mac).
    - Short press/release knob 1 to play/resume
    - Long press/release knob 1 to skip to next track
    - Rotate knob 1 to adjust volume up and down
    - Short press/release knob 2 to do open OldReader item (b)
    - Long press/release knob 1 to do star OpenReader post (f)
    - Rotate knob 2 to do navigate OldReader next/previous (n/p)

Author
  Dave MacLeod damacleod@gmail.com
Date
  11/FEB/2023
Credits:
  Nico Hood for HID-Project library - https://github.com/NicoHood/HID
  alizaliz's Giant USB Volume Knob - https://www.instructables.com/Giant-USB-Volume-Knob/
  Wolfgang Ziegler - https://wolfgang-ziegler.com/blog/pc-volume-control
*/

#include <HID-Project.h>
#include <HID-Settings.h>
#include <Encoder.h>

//components
Encoder encKnob(7, 6);  // encoder knob
const int encSwitch = 5; // pause/play button

// variables
long encPrevRotationValue  = -999; 
long encRotationValue = -999;
long encScale = 5;
int encSwitchState = 0;
bool encSwitchPressed = false;
unsigned long encSwitchPressTime = 0;
int debounce = 50;
unsigned long LongPressThreshold = 500;

// initialise 
void setup()
{
  pinMode(encSwitch, INPUT_PULLUP);
  Consumer.begin();
//  Serial.begin(9600); //for debugging
//  Serial.println("Serial monitor started");
}

//main loop
void loop()
{
  // do the Switch stuff
  encSwitchState = digitalRead(encSwitch); //update encoder switch state
  if (!encSwitchPressed && encSwitchState == HIGH) // Simple button press for knob momentary switch
  { 
    encSwitchPressed = true; // Only send command once per press
    Serial.println("Released");
    Serial.println(millis()-encSwitchPressTime);
    if (millis()-encSwitchPressTime>LongPressThreshold) // switch has been released - check the duration between press and release if more than threshold then skip track otherwise play/pause
    {
      Consumer.write(MEDIA_NEXT);
    }
    else
    {
      Consumer.write(MEDIA_PLAY_PAUSE);
    }
    encSwitchPressTime = 0;
  }
  if (encSwitchPressed && encSwitchState ==LOW)
  {
    encSwitchPressed = false; // reset switch status if released
    encSwitchPressTime = millis();
  }
  // do the rotary encoder1 stuff
  encRotationValue = (encKnob.read()/encScale); // read encoder position
  if (encRotationValue != encPrevRotationValue) //volume change
  { 
    if (encRotationValue < encPrevRotationValue) //turned anticlockwise
    { 
      Consumer.write(MEDIA_VOLUME_DOWN);
    }
    else
    {
      Consumer.write(MEDIA_VOLUME_UP);
    }
    encPrevRotationValue = encRotationValue;
  }
  delay(debounce); // soften control response
}
