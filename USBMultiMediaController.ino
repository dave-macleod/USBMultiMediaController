/*
Name
  USB Multimedia Controller
Purpose
  Using Arduino ATMega32u4 5v, aka Pro Micro, take input from rotary encoder with switch and mimic a USB HID Keyboard to send the commands to the connected computer (PC or Mac).
    - Short press/release knob to play/pause
    - Long press/release knob to skip to next track
    - Rotate knob to adjust volume up and down

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
const int wakeSwitch = 8;
// variables
long encPrevRotationValue  = -999; 
long encRotationValue = -999;
long encScale = 3.3;   // You will need to experiment with this value - I wanted a single click on my encoder to match a single increment of volume on my Mac Mini (M1)
int encSwitchState = 0;
bool encSwitchPressed = false;
unsigned long encSwitchPressTime = 0;
int debounce = 50;
unsigned long LongPressThreshold = 500;

int wakeSwitchState = 0;
bool wakeSwitchPressed = false;
bool debugOn = false;

// initialise 
void setup()
{
  pinMode(encSwitch, INPUT_PULLUP);
  pinMode(wakeSwitch, INPUT_PULLUP);
  Consumer.begin();
  if (debugOn)
  {
    Serial.begin(9600); //for debugging
    Serial.println("Serial monitor started");
  }
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

  // do the buttonpress
  wakeSwitchState = digitalRead(wakeSwitch);
  if (wakeSwitchPressed && wakeSwitchState == HIGH)   // switch was pressed and now isn't
  {
    wakeSwitchPressed = false; 
  }
  if (!wakeSwitchPressed && wakeSwitchState ==LOW) //switch wasn't pressed and now is
  {
    if (debugOn)
    {
      Serial.println("Wake up");
    }
    wakeSwitchPressed = true;
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.send();
    Keyboard.releaseAll();
  }
  delay(debounce); // soften control response
}
