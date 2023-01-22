/*struct inputData {
  int* flexion;
  int joyX;
  int joyY;
  bool joyClick;
  bool triggerButton;
  bool aButton;
  bool bButton;
  bool grab;
  bool pinch;
};

struct outputData{
  int* hapticLimits;
};
*/

char* debugSig(int sinRaw, int cosRaw, int sinMin, int sinMax, int cosMin, int cosMax, double sinScaled, double cosScaled, double angleRaw, double angleOrigin, double totalAngle, int indeg, int scaletot) {
  static char stringToEncode[200];
  sprintf(stringToEncode, "SIN[%d,%d,%d,%.3f] COS[%d,%d,%d,%.3f] ANG[%.3f,%.3f] TOTANG[%.3f, %d, %d]\n", sinRaw, sinMin, sinMax, sinScaled, cosRaw,  cosMin, cosMax, cosScaled, angleRaw, angleOrigin, totalAngle, indeg, scaletot);
  return stringToEncode;
}

char* debugFingerPos(int rawFinger, int minFinger, int maxFinger, int fingerPos) {
  static char stringToEncode[100];
  sprintf(stringToEncode, "raw[%d] min[%d] max[%d] Final[%d]\n", rawFinger, minFinger, maxFinger, fingerPos);
  return stringToEncode;
}

char* debugout(int* data) {
  static char stringToEncode[200];

  sprintf(stringToEncode, "T[%4d %4d %4d] I[%4d %4d %4d] M[%4d %4d %4d] R[%4d %4d %4d] P[%4d %4d %4d]\n", 
  data[0], data[5], data[10], data[1], data[6], data[11], data[2], data[7], data[12], data[3],data[8], data[13], data[14], data[9], data[14]);

//  sprintf(stringToEncode, "T=%4d I=%4d M=%4d R=%4d P=%4d   T=%4d I=%4d M=%4d R=%4d P=%4d   T=%4d I=%4d M=%4d R=%4d P=%4d\n", 
//  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9],data[10], data[11], data[12], data[13], data[14]);
  return stringToEncode;
}


#if ENCODING == ENCODING_LEGACY
//legacy encoding
char* encode(int* flexion, int joyX, int joyY, bool joyClick, bool triggerButton, bool aButton, bool bButton, bool grab, bool pinch, bool calib, bool menu){
  static char stringToEncode[75];
  
  sprintf(stringToEncode, "%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d\n", 
  flexion[0], flexion[1], flexion[2], flexion[3], flexion[4],
  joyX, joyY, joyClick,
  triggerButton, aButton, bButton, grab, pinch
  );
  return stringToEncode;
}
//legacy decoding
void decodeData(char* stringToDecode, int* hapticLimits){
    byte index = 0;
    char* ptr = strtok(stringToDecode, "&");  // takes a list of delimiters
    while(ptr != NULL)
    {
        hapticLimits[index] = atoi(ptr);
        index++;
        ptr = strtok(NULL, "&");  // takes a list of delimiters
    }
}
#endif

#if ENCODING == ENCODE_ALPHA
//alphabetic encoding
char* encode(int* flexion, int joyX, int joyY, bool joyClick, bool triggerButton, bool aButton, bool bButton, bool grab, bool pinch, bool calib, bool menu){
  static char stringToEncode[75];
  int trigger = (flexion[1] > ANALOG_MAX/2) ? (flexion[1] - ANALOG_MAX/2) * 2:0;
  #if USING_SPLAY
  sprintf(stringToEncode, "A%dB%dC%dD%dE%dF%dG%dP%d%s%s%s%s%s%s%s%s(AB)%d(BB)%d(CB)%d(DB)%d(EB)%d\n", 
  flexion[0], flexion[1], flexion[2], flexion[3], flexion[4],
  joyX, joyY, trigger, joyClick?"H":"",
  triggerButton?"I":"", aButton?"J":"", bButton?"K":"", grab?"L":"", pinch?"M":"", menu?"N":"", calib?"O":"",
  flexion[5], flexion[6], flexion[7], flexion[8], flexion[9]
  );
  #else
  sprintf(stringToEncode, "A%dB%dC%dD%dE%dF%dG%dP%d%s%s%s%s%s%s%s%s\n", 
  flexion[0], flexion[1], flexion[2], flexion[3], flexion[4],
  joyX, joyY, trigger, joyClick?"H":"",
  triggerButton?"I":"", aButton?"J":"", bButton?"K":"", grab?"L":"", pinch?"M":"", menu?"N":"", calib?"O":""
  );
  #endif
  return stringToEncode;
}

//legacy decoding
void decodeData(char* stringToDecode, int* hapticLimits){
  hapticLimits[0] = min(THUMB_SERV_MIN + getArgument(stringToDecode, 'A'), 1000); //thumb
  hapticLimits[1] = min(INDEX_SERV_MIN + getArgument(stringToDecode, 'B'), 1000); //index
  hapticLimits[2] = min(MIDDLE_SERV_MIN + getArgument(stringToDecode, 'C'), 1000); //middle
  hapticLimits[3] = min(RING_SERV_MIN + getArgument(stringToDecode, 'D'), 1000); //ring
  hapticLimits[4] = min(PINKY_SERV_MIN + getArgument(stringToDecode, 'E'), 1000); //pinky
  //Serial.println("Haptic: "+ (String)hapticLimits[0] + " " + (String)hapticLimits[1] + " " + (String)hapticLimits[2] + " " + (String)hapticLimits[3] + " " + (String)hapticLimits[4] + " ");
}

int getArgument(char* stringToDecode, char command){
  char* start = strchr(stringToDecode, command);
  if (start == NULL)
    return -1;
  else
    return atoi(start + 1);
}

#endif
