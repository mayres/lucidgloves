/*
 * LucidGloves Firmware Version 4
 * Author: Lucas_VRTech - LucidVR
 * lucidvrtech.com
 */

#include "ConfigUtils.h"
#include "AdvancedConfig.h"


//This is the configuration file, main structure in _main.ino
//CONFIGURATION SETTINGS:
//#define COMMUNICATION COMM_SERIAL //Which communication protocol to use. Options are: COMM_SERIAL (usb), COMM_BTSERIAL (bluetooth)
#define COMMUNICATION COMM_BTSERIAL //Which communication protocol to use. Options are: COMM_SERIAL (usb), COMM_BTSERIAL (bluetooth)
//serial over USB
  #define SERIAL_BAUD_RATE 115200
  
//serial over Bluetooth
  #define BTSERIAL_DEVICE_NAME "lucidgloves-left"

//ANALOG INPUT CONFIG
#define USING_SPLAY true //whether or not your glove tracks splay. - tracks the side to side "wag" of fingers. Requires 5 more inputs.
#define USING_MULTIPLEXER true //Whether or not you are using a multiplexer for inputs
#define FLIP_FLEXION  false  //Flip values from potentiometers (for fingers!) if they are backwards
#define FLIP_SPLAY true //Flip values for splay


//Gesture enables, make false to use button override
#define TRIGGER_GESTURE true
#define GRAB_GESTURE    true
#define PINCH_GESTURE   true


//BUTTON INVERT
//If a button registers as pressed when not and vice versa (eg. using normally-closed switches),
//you can invert their behaviour here by setting their line to true.
//If unsure, set to false
#define INVERT_A false
#define INVERT_B false
#define INVERT_JOY false
#define INVERT_MENU false
#define INVERT_CALIB false
//These only apply with gesture button override:
#define INVERT_TRIGGER false
#define INVERT_GRAB false
#define INVERT_PINCH false


//joystick configuration
#define JOYSTICK_BLANK true //make true if not using the joystick
#define JOY_FLIP_X false
#define JOY_FLIP_Y false
#define JOYSTICK_DEADZONE 10 //deadzone in the joystick to prevent drift (in percent)

#define NO_THUMB false //If for some reason you don't want to track the thumb

#define USING_CALIB_PIN true //When PIN_CALIB is shorted (or it's button pushed) it will reset calibration if this is on.

#define USING_FORCE_FEEDBACK true //Force feedback haptics allow you to feel the solid objects you hold
#define FLIP_FORCE_FEEDBACK true
#define SERVO_SCALING false //dynamic scaling of servo motors

#if defined(ESP32)
  //(This configuration is for ESP32 DOIT V1 so make sure to change if you're on another board)

  #define PIN_JOY_X     34
  #define PIN_JOY_Y     34
  #define PIN_JOY_BTN   34
  #define PIN_A_BTN     34 
  #define PIN_B_BTN     34
  #define PIN_TRIG_BTN  12 //unused if gesture set
  #define PIN_GRAB_BTN  13 //unused if gesture set
  #define PIN_PNCH_BTN  23 //unused if gesture set
  #define PIN_CALIB     22 //button for recalibration (You can set this to GPIO0 to use the BOOT button, but only when using Bluetooth.)
  #define DEBUG_LED 21

  #define PIN_PINKY_MOTOR     16  //used for force feedback
  #define PIN_RING_MOTOR      17  //^
  #define PIN_MIDDLE_MOTOR    5 //^
  #define PIN_INDEX_MOTOR     18  //^
  #define PIN_THUMB_MOTOR     19  //^
  #define PIN_MENU_BTN        34

  #define FLEXION_MIXING MIXING_SINCOS

  //To use a pin on the multiplexer, use MUX(pin). So for example pin 15 on a mux would be MUX(15).
  #define PIN_PINKY         MUX(0) //These 5 are for flexion
  #define PIN_PINKY_SPLAY   MUX(1)
  #define PIN_PINKY_SECOND  MUX(2) 

  #define PIN_RING          MUX(3)
  #define PIN_RING_SPLAY    MUX(4)
  #define PIN_RING_SECOND   MUX(5)
  
  #define PIN_MIDDLE        MUX(6)
  #define PIN_MIDDLE_SPLAY  MUX(7)
  #define PIN_MIDDLE_SECOND MUX(8)
  
  #define PIN_INDEX         MUX(9)
  #define PIN_INDEX_SPLAY   MUX(10)
  #define PIN_INDEX_SECOND  MUX(11)
 
  #define PIN_THUMB         MUX(12)
  #define PIN_THUMB_SPLAY   MUX(13)
  #define PIN_THUMB_SECOND  MUX(14)

  //Select pins for multiplexers, set as needed if using a mux. You can add or remove pins as needed depending on how many select pins your mux needs.
  #define PINS_MUX_SELECT     26,  /*S0 pin*/ \
                              25,  /*S1 pin*/ \
                              33,  /*S2 pin*/ \
                              32   /*S3 pin (if your mux is 3-bit like 74HC4051 then you can remove this line and the backslash before it.)*/
  
  #define MUX_INPUT 35  //the input or SIG pin of the multiplexer. This can't be a mux pin.

#endif
