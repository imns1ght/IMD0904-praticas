// Pratica 01

// #include <SoftwareSerial.h>

// SoftwareSerial ArduinoSlave(10, 11);
// char cmd = "";
// char old_cmd;
// char answer = "";
// char old_answer;

// void setup() {
//   Serial.begin(9600);
//   Serial.println("ENTER Commands: ");
//   ArduinoSlave.begin(9600);
// }

// void loop() {
//   old_cmd = cmd;
//   old_answer = answer;  
//   // Read command from monitor
//   if (Serial.available()) {
//     cmd = Serial.read();
//   }

//   // Read answer from slave
//   if (ArduinoSlave.available()) {
//     answer = ArduinoSlave.read();
//   }

//   // Send data to slave
//   if (cmd != old_cmd) {
//     Serial.print("Master send: ");
//     Serial.println(cmd);
//     ArduinoSlave.write(cmd);
//   }

//   // Send answer to monitor
//   if (answer != old_answer) {
//     Serial.print("Slave received: ");
//     Serial.println(answer);
//   }
// }

// Pratica 02

#include <SoftwareSerial.h>

SoftwareSerial ArduinoSlave(10, 11);
String answer;
String msg;
int intVal = 0, oldIntVal = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("ENTER Commands: ");
  ArduinoSlave.begin(9600);
}

void loop() {
  // Read sensor
  intVal = analogRead(A0);
  // Read answer from slave
  readSlavePort();

  // Send data to slave
  if (oldIntVal != intVal) {
    Serial.print("Master sent: ");
    Serial.println(intVal);
    ArduinoSlave.print(intVal);
    oldIntVal = intVal;
  }

  // Send answer to monitor
  if (answer != "") {
    Serial.print("Slave LED PWM value: ");
    Serial.println(answer);
    answer = "";
  }
  delay(1000);  
}

void readSlavePort() {
  while (ArduinoSlave.available()) {
    delay(10);
    if (ArduinoSlave.available() > 0) {
      char c = ArduinoSlave.read();
      answer += c;
    }
  }
}

