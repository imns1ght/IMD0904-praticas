// Pratica 01 (Slave)

// #include <SoftwareSerial.h>
// SoftwareSerial ArduinoMaster(10,11);
// char cmd="";
// char old_cmd;

// void setup() {
//   ArduinoMaster.begin(9600);
// }

// void loop() {
//   old_cmd=cmd;
//   if(ArduinoMaster.available()){
//     cmd=ArduinoMaster.read();  
//   }
//   if(cmd!=old_cmd){
//     ArduinoMaster.write(cmd);
//   }
// }

#include <SoftwareSerial.h>
SoftwareSerial ArduinoMaster(10,11);
#define ledPin 11
String msg ="";
int ledVal=0;
int intVal=0,oldIntVal =0;

void setup() {
  char cmd ="";
  ArduinoMaster.begin(9600);
  pinMode(ledPin,OUTPUT);
}

void loop() {
  readMasterPort();
  convertMsgToCmd();
  // Send answer to master
  if (intVal != oldIntVal) {
    Serial.print("Master sent: ");
    Serial.println(intVal);

    ledVal = map(intVal, 0, 1023, 0, 255);
    Serial.print("led value: ");
    Serial.println(ledVal);
    ArduinoMaster.print(ledVal);
    analogWrite(ledPin, ledVal);
    oldIntVal = intVal;
  }
}

void readMasterPort() {
  while(ArduinoMaster.available()) {
    delay(10);
    if (ArduinoMaster.available() > 0) {
      char c = ArduinoMaster.read();
      msg += c;
    }
  }
  ArduinoMaster.flush();
}

void convertMsgToCmd() {
  if (msg.length() > 0) {
    Serial.print("message length: ");
    Serial.println(msg.length());

    char carrayl[6];
    msg.toCharArray(carrayl, sizeof(carrayl));
    intVal = atoi(carrayl);

    msg = "";
  }
}
