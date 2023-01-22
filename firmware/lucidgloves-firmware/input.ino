#include "esp_adc_cal.h"
// Requires RunningMedian library by Rob Tillaart
#if (ENABLE_MEDIAN_FILTER || ((INTERFILTER_MODE != INTERFILTER_NONE) && (FLEXION_MIXING != MIXING_NONE)))
  #include <RunningMedian.h>
#endif

#if ENABLE_MEDIAN_FILTER
  RunningMedian rmSamples[10] = {
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES),
      RunningMedian(MEDIAN_SAMPLES)
  };
#endif

#if ((INTERFILTER_MODE != INTERFILTER_NONE) && (FLEXION_MIXING == MIXING_SINCOS))
  RunningMedian sinSamples[5] = {
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES)
  };

    RunningMedian cosSamples[5] = {
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES),
      RunningMedian(INTERFILTER_SAMPLES)
  };
#endif

#define DEFAULT_VREF 1100
esp_adc_cal_characteristics_t *adc_chars;

byte selectPins[] = {PINS_MUX_SELECT};

int maxFingers[10] = {0,0,0,0,0,0,0,0,0,0};
int minFingers[10] = {ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX};

#if FLEXION_MIXING == MIXING_SINCOS
  #if INTERMEDIATE_CALIBRATION
  int sinMin[5] = {ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX};
  int sinMax[5] = {0,0,0,0,0};

  int cosMin[5] = {ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX};
  int cosMax[5] = {0,0,0,0,0};
  #else
  int sinMax[5] = {INTER_MAX, INTER_MAX, INTER_MAX, INTER_MAX, INTER_MAX};
  int sinMin[5] = {INTER_MIN, INTER_MIN, INTER_MIN, INTER_MIN, INTER_MIN};

  int cosMax[5] = {INTER_MAX, INTER_MAX, INTER_MAX, INTER_MAX, INTER_MAX};
  int cosMin[5] = {INTER_MIN, INTER_MIN, INTER_MIN, INTER_MIN, INTER_MIN};
  #endif

  bool atanPositive[8] = {true, true, true, true, true, true, true, true};


  int totalOffset1[5] = {0,0,0,0,0};
  double startAngle[5] = {0,0,0,0,0};
#endif

void setupInputs(){
  pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_A_BTN, INPUT_PULLUP);
  pinMode(PIN_B_BTN, INPUT_PULLUP);

  pinMode(PIN_MENU_BTN, INPUT_PULLUP);
  
  #if !TRIGGER_GESTURE
  pinMode(PIN_TRIG_BTN, INPUT_PULLUP);
  #endif

  #if !GRAB_GESTURE
  pinMode(PIN_GRAB_BTN, INPUT_PULLUP);
  #endif

  #if !PINCH_GESTURE
  pinMode(PIN_PNCH_BTN, INPUT_PULLUP);
  #endif

  #if USING_CALIB_PIN
  pinMode(PIN_CALIB, INPUT_PULLUP);
  #endif

  #if USING_MULTIPLEXER
  byte selectPins[] = {PINS_MUX_SELECT};
  //pinMode(MUX_INPUT, INPUT);
  for (int i = 0; i < sizeof(selectPins); i++){
    pinMode(selectPins[i], OUTPUT);
  }
  #endif
}

int analogPinRead(int pin){
  #if USING_MULTIPLEXER
  if (ISMUX(pin)){
    return readMux(UNMUX(pin));
  }
  else{
    return analogRead(pin);
  }
  #else
   return analogRead(UNMUX(pin));
  #endif
}

#if USING_MULTIPLEXER
int readMux(byte pin){
  /*byte selectPins[] = {PINS_MUX_SELECT}; //get the array of select pins for the mux

  for (int i = sizeof(selectPins - 1); i > -1; i--){
    digitalWrite(selectPins[i], ((int)pow(2,i) & (pin)) == 0 ? LOW:HIGH); //convert the pin number to binary, and set each digit to it's corresponsing select pin.
  }

  */
  switch(pin){
    case 0:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], LOW);
      break;
    case 1:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], LOW);
      break;
    case 2:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], LOW);
      break;
   case 3:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], LOW);
      break;
   case 4:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], LOW);
      break;
    case 5:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], LOW);
      break;
    case 6:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], LOW);
      break;
    case 7:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], LOW);
      break;
    case 8:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], HIGH);
      break;
    case 9:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], HIGH);
      break;
   case 10:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], HIGH);
      break;
    case 11:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], LOW);
      digitalWrite(selectPins[3], HIGH);
      break;
    case 12:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], HIGH);
      break;
    case 13:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], LOW);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], HIGH);
      break;
    case 14:
      digitalWrite(selectPins[0], LOW);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], HIGH);
      break;
   case 15:
      digitalWrite(selectPins[0], HIGH);
      digitalWrite(selectPins[1], HIGH);
      digitalWrite(selectPins[2], HIGH);
      digitalWrite(selectPins[3], HIGH);
      break;
  }
  delayMicroseconds(MULTIPLEXER_DELAY);
  int rC = analogRead(MUX_INPUT);

  int retInt = analogRead(MUX_INPUT);
  switch(MUX(pin)) {
    case PIN_THUMB:
      debugData[0] = retInt;    
      break;
    case PIN_INDEX:
      debugData[1] = retInt;    
      break;
    case PIN_MIDDLE:
      debugData[2] = retInt;    
      break;
    case PIN_RING:
      debugData[3] = retInt;    
      break;
    case PIN_PINKY:
      debugData[4] = retInt;    
      break;
    case PIN_THUMB_SECOND:
      debugData[5] = retInt;    
      break;
    case PIN_INDEX_SECOND:
      debugData[6] = retInt;    
      break;
    case PIN_MIDDLE_SECOND:
      debugData[7] = retInt;    
      break;
    case PIN_RING_SECOND:
      debugData[8] = retInt;    
      break;
    case PIN_PINKY_SECOND:
      debugData[9] = retInt;    
      break;
    case PIN_THUMB_SPLAY:
      debugData[10] = retInt;    
      break;
    case PIN_INDEX_SPLAY:
      debugData[11] = retInt;    
      break;
    case PIN_MIDDLE_SPLAY:
      debugData[12] = retInt;    
      break;
    case PIN_RING_SPLAY:
      debugData[13] = retInt;    
      break;
    case PIN_PINKY_SPLAY:
      debugData[14] = retInt;    
      break;
  }
  return retInt;
}
#endif

void getFingerPositions(bool calibrating, bool reset){
  #if FLEXION_MIXING == MIXING_NONE //no mixing, just linear
  int rawFingersFlexion[5] = {NO_THUMB?0:analogPinRead(PIN_THUMB), analogPinRead(PIN_INDEX), analogPinRead(PIN_MIDDLE), analogPinRead(PIN_RING), analogPinRead(PIN_PINKY)};
  
  #elif FLEXION_MIXING == MIXING_SINCOS
  int rawFingersFlexion[5] = {NO_THUMB?0:sinCosMix(PIN_THUMB, PIN_THUMB_SECOND, 0 ), 
                                  sinCosMix(PIN_INDEX, PIN_INDEX_SECOND, 1 ), 
                                  sinCosMix(PIN_MIDDLE,PIN_MIDDLE_SECOND,2 ), 
                                  sinCosMix(PIN_RING,  PIN_RING_SECOND,  3 ), 
                                  sinCosMix(PIN_PINKY, PIN_PINKY_SECOND, 4 )};

  #endif

  int rawFingers[10];

  #if USING_SPLAY
    int rawFingersSplay[5] = {NO_THUMB?0:analogPinRead(PIN_THUMB_SPLAY), 
                              analogPinRead(PIN_INDEX_SPLAY), 
                              analogPinRead(PIN_MIDDLE_SPLAY), 
                              analogPinRead(PIN_RING_SPLAY), 
                              analogPinRead(PIN_PINKY_SPLAY)};
  #else
    int rawFingersSplay[5] = {0,0,0,0,0};
  #endif
    //memcpy(rawFingers, rawFingersFlexion, 5); //memcpy doesn't seem to work here
    //memcpy(&rawFingers[5], rawFingersSplay, 5); 

  for (int i = 0; i < 5; i++){
    rawFingers[i] = rawFingersFlexion[i];
    rawFingers[i+5] = rawFingersSplay[i];
  }
  
  
  //flip pot values if needed
  #if FLIP_FLEXION
  for (int i = 0; i < 5; i++){
    rawFingers[i] = ANALOG_MAX - rawFingers[i];
  }
  #endif
  
  #if FLIP_SPLAY
  for (int i = 5; i < 10; i++){
    rawFingers[i] = ANALOG_MAX - rawFingers[i];
  }
  #endif
  
  #if ENABLE_MEDIAN_FILTER
  for (int i = 0; i < 10; i++){
    rmSamples[i].add( rawFingers[i] );
    rawFingers[i] = rmSamples[i].getMedian();
  }
  #endif

  //reset max and mins as needed
  if (reset){
    for (int i = 0; i <10; i++){
      #if FLEXION_MIXING == MIXING_SINCOS
      if (i < 5)
        totalOffset1[i] = 0;
      #endif
      maxFingers[i] = INT_MIN;
      minFingers[i] = INT_MAX;
    }
  }
  
  //if during the calibration sequence, make sure to update max and mins
  if (calibrating){
    for (int i = 0; i <10; i++){
      if (rawFingers[i] < 4094 && rawFingers[i] > maxFingers[i])
        #if CLAMP_SENSORS
          maxFingers[i] = ( rawFingers[i] <= CLAMP_MAX )? rawFingers[i] : CLAMP_MAX;
        #else
          maxFingers[i] = rawFingers[i];
        #endif
      if (rawFingers[i] > 0 && rawFingers[i] < minFingers[i])
        #if CLAMP_SENSORS
          minFingers[i] = ( rawFingers[i] >= CLAMP_MIN )? rawFingers[i] : CLAMP_MIN;
        #else
          minFingers[i] = rawFingers[i];
        #endif
    }
  }
  
  for (int i = 0; i<10; i++){
    if (minFingers[i] != maxFingers[i]){
      fingerPos[i] = map( rawFingers[i], minFingers[i], maxFingers[i], 0, ANALOG_MAX );
      #if CLAMP_ANALOG_MAP
        if (fingerPos[i] < 0)
          fingerPos[i] = 0;
        if (fingerPos[i] > ANALOG_MAX)
          fingerPos[i] = ANALOG_MAX;
      #endif
    }
    else {
      fingerPos[i] = ANALOG_MAX / 2;
    }
    
  }
}

int analogReadDeadzone(int pin){
  int raw = analogPinRead(pin);
  if (abs(ANALOG_MAX/2 - raw) < JOYSTICK_DEADZONE * ANALOG_MAX / 100)
    return ANALOG_MAX/2;
  else
    return raw;
}

int getJoyX(){
  #if JOYSTICK_BLANK
  return ANALOG_MAX/2;
  #elif JOY_FLIP_X
  return ANALOG_MAX - analogReadDeadzone(PIN_JOY_X);
  #else
  return analogReadDeadzone(PIN_JOY_X);
  #endif
}

int getJoyY(){
  #if JOYSTICK_BLANK
  return ANALOG_MAX/2;
  #elif JOY_FLIP_Y
  return ANALOG_MAX - analogReadDeadzone(PIN_JOY_Y);
  #else
  return analogReadDeadzone(PIN_JOY_Y);
  #endif
}

bool getButton(byte pin){
  return digitalRead(pin) != HIGH;
}

#if FLEXION_MIXING == MIXING_SINCOS
//mixing
int sinCosMix(int sinPin, int cosPin, int i){

  int sinRaw = analogPinRead(sinPin);
  int cosRaw = analogPinRead(cosPin);


  #if INTERFILTER_MODE != INTERFILTER_NONE
    sinSamples[i].add(sinRaw);
    cosSamples[i].add(cosRaw);
    int sinCalib = sinSamples[i].getMedian();
    int cosCalib = cosSamples[i].getMedian();
    #if INTERFILTER_MODE == INTERFILTER_ALL
      sinRaw = sinCalib;
      cosRaw = cosCalib;
    #endif
  #else
    int sinCalib = sinRaw;
    int cosCalib = cosRaw;
  #endif 

  #if INTERMEDIATE_CALIBRATION
  //scaling
  sinMin[i] = min(sinCalib, sinMin[i]);
  sinMax[i] = max(sinCalib, sinMax[i]);

  cosMin[i] = min(cosCalib, cosMin[i]);
  cosMax[i] = max(cosCalib, cosMax[i]);
  #endif

  int sinScaled = map(sinRaw, sinMin[i], sinMax[i], -ANALOG_MAX, ANALOG_MAX);
  int cosScaled = map(cosRaw, cosMin[i], cosMax[i], -ANALOG_MAX, ANALOG_MAX);


  //trigonometry stuffs
  double angleRaw = -atan2(sinScaled, cosScaled);

  if(calibReset) {
    startAngle[i] = angleRaw;
    totalOffset1[i] = 0;
  }

  double angleOrigin = angleRaw - startAngle[i];
  //counting rotations
  if (((angleRaw > 0) != atanPositive[i]) && sinScaled > cosScaled){
    totalOffset1[i] += atanPositive[i]?1:-1;
  }
  atanPositive[i] = angleRaw > 0;
  double totalAngle = angleOrigin + 2*PI * totalOffset1[i];
  
  int totindeg = totalAngle * RAD_TO_DEG;
  int scaletotal = map(totindeg, -45, 225, 0, ANALOG_MAX);

  if(i == 4) {
    comm->output(debugSig(sinRaw, cosRaw, sinMin[i], sinMax[i], cosMin[i], cosMax[i], sinScaled, cosScaled, angleRaw, angleOrigin, totalAngle, totindeg, scaletotal));
  } 

  return (int)(totalAngle * ANALOG_MAX);
  
}
#endif
