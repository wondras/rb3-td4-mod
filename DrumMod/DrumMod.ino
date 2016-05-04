/*
  DrumMod
  Enables use Roland TD-4 hi-hat pedal with Rock Band Pro MIDI Adapter

  2013-03-17
  Warren Ondras
  wondras@gmail.com

  WIRING:
    Cut trace from pin on MCU that connects to PC900V optocoupler pin 4
      (do not cut between opto and R114)
    Connections on Arduino Pro Mini:
      pin 0 (RXI) to pin 4 of PC900V
      pin 1 (TXO) to pin on MCU
      pin 2 to mode switch pin 3 (as viewed from bottom)
      Vcc to 3.3V pad
      GND to GND pad
    Optional - connect pin 0 through a 2.2K resistor
      (may prevent MIDI signals from interfering with serial code upload)
      
   
  FUTURE:
    - cymbal double-trigger suppression (drop second note if edge/bow both trigger
    - flams?
*/

// Arduino pins
const int MODE_SWITCH = 2;
const int STATUS_LED = 13;

// MIDI commands
const byte NOTE_ON = 153;
const byte NOTE_OFF = 137;

// Roland TD-4 default note map
const byte TOM1 = 48; // high tom
const byte TOM2 = 45; // mid tom
const byte TOM3 = 41; // low tom
const byte SNARE_HEAD = 38;
const byte SNARE_RIM = 40;
const byte KICK = 36;
const byte RIDE_BOW = 51;
const byte RIDE_EDGE = 59;
const byte HH_OPEN_BOW = 46;
const byte HH_OPEN_EDGE = 26;
const byte HH_CLOSED_BOW = 42;
const byte HH_CLOSED_EDGE = 22;
const byte HH_FOOT_CLOSE = 44;
const byte CRASH1_BOW = 49;
const byte CRASH1_EDGE = 55;
const byte CRASH2_BOW = 57;
const byte CRASH2_EDGE = 52;

// Timing parameters
const int YELLOW_CRASH_WINDOW = 50; // max milliseconds between crash1/crash2 to count as yellow/green

void blink(int blinks=1, bool dah=false, int pin=13) {
  int i, delaylength;
  
  if (dah) {
    delaylength = 500; // dah
  } else {
    delaylength = 250; // dit
  }
  
  for (int i=0; i<blinks; i++) {
    digitalWrite(pin, HIGH);
    delay(delaylength);
    digitalWrite(pin, LOW);
    delay(delaylength); 
  }
}

void setup() { 
  //  Set MIDI baud rate:
  Serial.begin(31250);
  
  pinMode(STATUS_LED, OUTPUT);
  pinMode(MODE_SWITCH, INPUT);
  
  blink(3); // identify program with three short blinks
}
   
void loop() {  
  byte inbyte, outbyte;
  byte lastbyte = 0x00;
  byte ledstate = 0;
  
  byte yellow_crash_note;
  unsigned long last_crash1_time = 0;
  unsigned long last_crash2_time = 0;
  bool yellow_crash_active = false;
  
  // drum mode
  while (digitalRead(MODE_SWITCH)==LOW) {
    if (Serial.available() > 0) {
      inbyte = Serial.read();
      outbyte = inbyte;
      
      // open hi-hat redirect
      if ( ((lastbyte==NOTE_ON) or (lastbyte==NOTE_OFF))
           and ((inbyte==HH_OPEN_BOW) or (inbyte==HH_OPEN_EDGE)) ) {
        outbyte = RIDE_EDGE;
      }  
      
      // TO-DO: filter simultaneous bow/edge hits here
            
      // map simultaneous crash1/crash2 hits to yellow/green instead of two green notes
      // (if crash hit, and the other crash was hit just recently, send a yellow note instead of green)      
      if ( (lastbyte==NOTE_ON) and ((inbyte==CRASH1_BOW) or (inbyte==CRASH1_EDGE)) ) {
        last_crash1_time = millis();
        if ( (yellow_crash_active == false) and 
             ((last_crash1_time - last_crash2_time) < YELLOW_CRASH_WINDOW) ) {
          yellow_crash_active = true;
          yellow_crash_note = inbyte;
          outbyte = HH_CLOSED_EDGE;
        }
      }      
      if ( (lastbyte==NOTE_ON) and ((inbyte==CRASH2_BOW) or (inbyte==CRASH2_EDGE)) ) {
        last_crash2_time = millis();
        if ( (yellow_crash_active == false) and 
             ((last_crash2_time - last_crash1_time) < YELLOW_CRASH_WINDOW) ) {
          yellow_crash_active = true;
          yellow_crash_note = inbyte;
          outbyte = HH_CLOSED_EDGE;
        }
      }
      if ( (yellow_crash_active==true) and (lastbyte==NOTE_OFF) and (inbyte==yellow_crash_note) ) {
        yellow_crash_active = false;
        outbyte = HH_CLOSED_EDGE;
      }
            
      Serial.write(outbyte);
      lastbyte = inbyte;
      
      // show that there was some activity
      ledstate ^= 0x01;
      digitalWrite(STATUS_LED, ledstate);
    }
  }
  
  // when not in drum mode, just pass through
  while (digitalRead(MODE_SWITCH)==HIGH) {
    if (Serial.available() > 0) {
      inbyte = Serial.read();
      Serial.write(inbyte);
    }
  }
    
}


