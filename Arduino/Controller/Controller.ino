#include <Bluepad32.h>
#include <ESP32Servo.h>
#include <vector>

Servo myservo;
GamepadPtr gamepadList[BP32_MAX_GAMEPADS];

int greenLedAPin = 22;
int greenLedBPin = 21;
int whiteLedPin = 23;
int servoAPin = 5;
int buzzerPin = 4;

char cheatCode[] = { 'l', 'e', 'a', 'v', 'e', '\0' };

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedGamepad(GamepadPtr gp) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepadList[i] == nullptr) {
      Serial.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      GamepadProperties properties = gp->getProperties();
      Serial.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n",
                    gp->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      gamepadList[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
      "CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepadList[i] == gp) {
      Serial.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      gamepadList[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
      "CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}

void printDebug(GamepadPtr &myGamepad) {
  // Another way to query the buttons, is by calling buttons(), or
  // miscButtons() which return a bitmask.
  // Some gamepads also have DPAD, axis and more.
  Serial.printf(
    "dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: "
    "%4d, %4d, brake: %4d, throttle: %4d\n",
    myGamepad->dpad(),     // DPAD
    myGamepad->buttons(),  // bitmask of pressed buttons
    myGamepad->axisX(),    // (-511 - 512) left X Axis
    myGamepad->axisY(),    // (-511 - 512) left Y axis
    myGamepad->axisRX(),   // (-511 - 512) right X axis
    myGamepad->axisRY(),   // (-511 - 512) right Y axis
    myGamepad->brake(),    // (0 - 1023): brake button
    myGamepad->throttle()  // (0 - 1023): throttle (AKA gas) button
  );
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  BP32.forgetBluetoothKeys();

  myservo.attach(servoAPin);
  pinMode(greenLedAPin, OUTPUT);
  pinMode(greenLedBPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);
}

void resetLeds() {
  digitalWrite(whiteLedPin, LOW);
  digitalWrite(greenLedBPin, LOW);
  digitalWrite(greenLedAPin, LOW);
}

void horizontalAction(GamepadPtr &gamepad, Servo &servo) {
  int axisxValue = gamepad->axisX();
  int axisXMapped = map(axisxValue, -511, 512, 0, 180);
  servo.write(axisXMapped);
  bool isAxisXActivated = axisxValue < -5 || axisxValue > 5 ? true : false;

  if (isAxisXActivated) {
    digitalWrite(whiteLedPin, HIGH);
    // digitalWrite(greenLedAPin, HIGH);
    // digitalWrite(greenLedBPin, HIGH);
  }

  bool isAxisXMax = servo.read() == 0 || servo.read() == 180;
  if (isAxisXMax) {
    int buzzerFreq = 5000;
    tone(buzzerPin, buzzerFreq, 300);
  }
}

void cheatcodeAction() {
  digitalWrite(whiteLedPin, HIGH);
  digitalWrite(greenLedAPin, HIGH);
  digitalWrite(greenLedBPin, HIGH);
  tone(buzzerPin, 2500, 300);
  delay(1000);  
  resetLeds();
}

void loop() {
  BP32.update();
  std::vector<char> cheatInput;
  cheatcodeAction();

  // for (int i = 0; i < 6; i++) {
  //   if (cheatInput[i] == cheatCode[i]) {

  //   }
  // }

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    GamepadPtr gamepad = gamepadList[i];

    if (gamepad && gamepad->isConnected()) {
      resetLeds();
      horizontalAction(gamepad, myservo);
      printDebug(gamepad);

      if (gamepad->a()) {
        cheatInput.push_back('a');
      }
    }
  }
  delay(150);
}
