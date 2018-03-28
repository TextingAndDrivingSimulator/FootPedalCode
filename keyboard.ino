
#include <Keyboard.h>
// Don't bother with interrupts yet, not doing enough to justify them!
// Polling works fine, and is simpler to code up.
class KeyboardKey {
  public: 
  // pinH == The pin that is normally high
  // pinL == The pin that is normally low
  KeyboardKey(int pinH, int pinL, char keyToSend, int kbDebounceTime = 5, bool debug = false):pinHigh(pinH), pinLow(pinL), keyToSend(keyToSend), kbDebounceTime(kbDebounceTime), debug(debug) {
    pinMode(pinH,INPUT_PULLUP);
    pinMode(pinL,INPUT_PULLUP);
  }


  void update() {
    bool oldPinsAbnormal = (this->arePinsAbnormal());

    delay(kbDebounceTime); 

    // If the reading has changed since we started, we are bouncing, abort
    // wait for a stable reading
    if ( oldPinsAbnormal != this->arePinsAbnormal() ) {
      return;
    }

    // If the pedal is depressed, then if we haven't yet sent a key press, send one.
    // Else, if the pins are normal, and we haven't yet sent a key release, send one.
    if ( oldPinsAbnormal ) {
      if ( !sentKeyPressed ) {
        sendKeyPressed();
        sentKeyPressed = true;
      }
    } else {
      if ( sentKeyPressed ) {
        sendKeyReleased();
        sentKeyPressed = false;
      }
    }
    
  }

  private:

  bool arePinsAbnormal() {
    // If the pins are not in the state they normally are, that is abnormal.
    return digitalRead(pinHigh) == LOW && digitalRead(pinLow) == HIGH;
  }

  void sendKeyReleased() {
    if ( debug ) {
      Serial.print("Released key!\n");
    } else {
      Keyboard.release(keyToSend);
    }
  }

  void sendKeyPressed() {
    if ( debug ) {
      Serial.print("Pressed key!\n");
    } else {
      Keyboard.press(keyToSend);
    }
  }

  int pinLow;
  int pinHigh;
  char keyToSend;
  int kbDebounceTime;
  bool debug;

  bool sentKeyPressed = false;

  
};

KeyboardKey keyW(12,11,'w',5,false);
KeyboardKey keyS(10,9,'s',5,false);
void setup() {
  // put your setup code here, to run once:
  pinMode(A1,INPUT);
  Keyboard.begin();

  Serial.begin(9600);
  Serial.println("Starting serial!");

}





void loop() {
  keyW.update();
  keyS.update();
  delay(10);


  
  // Analog read is 0-1023, so we use two bytes to send the result over!
  short value = analogRead(A1);
  byte buff[2];
  buff[0] = value & 0x00FF;
  buff[1] = (value >> 8) & 0x00FF;
  Serial.write(buff,2);
  

}
