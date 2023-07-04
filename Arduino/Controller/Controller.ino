#include <Bluepad32.h>
#include <ESP32Servo.h>

Servo horizontalServo;
GamepadPtr gamepadList[BP32_MAX_GAMEPADS];

// Pins
int greenLedAPin = 22;
int greenLedBPin = 21;
int whiteLedPin = 23;
int servoAPin = 5;
int buzzerPin = 4;

// Chamado quando o gamepad é conectado.
// Até 4 gamepads podem ser conectados ao mesmo tempo.
void onConnectedGamepad(GamepadPtr gp) {
  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepadList[i] == nullptr) {
      Serial.printf("Gamepad conectado, index=%d\n", i);
      GamepadProperties properties = gp->getProperties();
      Serial.printf("Gamepad modelo: %s, VID=0x%04x, PID=0x%04x\n",
                    gp->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      gamepadList[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println(
      "Gamepad conectado, mas não foi encontrado um slot vazio.");
  }
}

// Chamado quando o gamepad é desconectado
void onDisconnectedGamepad(GamepadPtr gp) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepadList[i] == gp) {
      Serial.printf("Gamepad desconectado, index=%d\n", i);
      gamepadList[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
      "Gamepad desconectado, mas não encontrado na lista de gamepads.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  BP32.forgetBluetoothKeys();

  horizontalServo.attach(servoAPin);
  pinMode(greenLedAPin, OUTPUT);
  pinMode(greenLedBPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);
}

// Imprime os inputs do gamepad
void printDebug(GamepadPtr &gamepad) {
  Serial.printf(
    "dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: "
    "%4d, %4d, brake: %4d, throttle: %4d\n",
    gamepad->dpad(),     // DPAD
    gamepad->buttons(),  // bitmask of pressed buttons
    gamepad->axisX(),    // (-511 - 512) left X Axis
    gamepad->axisY(),    // (-511 - 512) left Y axis
    gamepad->axisRX(),   // (-511 - 512) right X axis
    gamepad->axisRY(),   // (-511 - 512) right Y axis
    gamepad->brake(),    // (0 - 1023): brake button
    gamepad->throttle()  // (0 - 1023): throttle (AKA gas) button
  );
}

void turnOffLeds() {
  digitalWrite(whiteLedPin, LOW);
  digitalWrite(greenLedBPin, LOW);
  digitalWrite(greenLedAPin, LOW);
}

void horizontalAction(GamepadPtr &gamepad) {
  int axisxValue = gamepad->axisX();
  int axisXMapped = map(axisxValue, -511, 512, 0, 180);
  horizontalServo.write(axisXMapped);
  bool isAxisXActivated = axisxValue < -5 || axisxValue > 5 ? true : false;

  if (isAxisXActivated) {
    digitalWrite(whiteLedPin, HIGH);
    digitalWrite(greenLedAPin, HIGH);
    digitalWrite(greenLedBPin, HIGH);
  }

  bool isAxisXMax = horizontalServo.read() == 0 || horizontalServo.read() == 180;
  if (isAxisXMax) {
    int buzzerFreq = 5000;
    tone(buzzerPin, buzzerFreq, 300);
  }
}

void yButtonAction() {
  // Faz o LED branco aumentar gradualmente de intensidade
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(whiteLedPin, brightness);
    delay(10);
  }

  // Movimenta o servo motor gradualmente para a posição 180
  for (int pos = 0; pos <= 180; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  digitalWrite(greenLedAPin, HIGH);
  digitalWrite(greenLedBPin, HIGH);
  tone(buzzerPin, 2500, 300);
  delay(1000);
  turnOffLeds();
}

void bButtonAction() {
  digitalWrite(whiteLedPin, HIGH);
  digitalWrite(greenLedAPin, HIGH);
  digitalWrite(greenLedBPin, HIGH);
  tone(buzzerPin, 2500, 300);
  delay(1000);

  // Movimenta o servo motor gradualmente para a posição 90
  for (int pos = 0; pos <= 90; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  // Faz o LED branco diminuir gradualmente de intensidade
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(whiteLedPin, brightness);
    delay(10);
  }

  turnOffLeds();
}

void aButtonAction() {
  // Movimenta o servo motor gradualmente para a posição 45
  for (int pos = 0; pos <= 45; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  digitalWrite(whiteLedPin, HIGH);
  digitalWrite(greenLedAPin, HIGH);
  digitalWrite(greenLedBPin, HIGH);
  tone(buzzerPin, 1500, 500);
  delay(1000);
  turnOffLeds();
}

void xButtonAction() {
  digitalWrite(whiteLedPin, HIGH);

  // Movimenta o servo motor gradualmente para a posição 90
  for (int pos = 0; pos <= 90; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  // Piscar os LEDs verdes cinco vezes
  for (int i = 0; i < 5; i++) {
    digitalWrite(greenLedAPin, HIGH);
    digitalWrite(greenLedBPin, HIGH);
    delay(500);
    digitalWrite(greenLedAPin, LOW);
    digitalWrite(greenLedBPin, LOW);
    delay(500);
  }

  tone(buzzerPin, 2500, 300);
  delay(1000);
  turnOffLeds();
}

void l1ButtonAction() {
  digitalWrite(whiteLedPin, HIGH);

  // Acender e apagar aleatoriamente os LEDs verdes cinco vezes
  for (int i = 0; i < 5; i++) {
    digitalWrite(greenLedAPin, random(2));
    digitalWrite(greenLedBPin, random(2));
    delay(500);
  }

  // Movimenta o servo motor gradualmente para a posição 180
  for (int pos = 0; pos <= 180; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  tone(buzzerPin, 2500, 300);
  delay(1000);
  turnOffLeds();
}

void r1ButtonAction() {
  for (int brilho = 0; brilho <= 255; brilho += 5) {
    analogWrite(whiteLedPin, brilho);
    delay(10);
  }

  tone(buzzerPin, 2500, 300);
  delay(1000);

  for (int brilho = 255; brilho >= 0; brilho -= 5) {
    analogWrite(whiteLedPin, brilho);
    delay(10);
  }

  // Movimenta o servo motor gradualmente para a posição 180
  for (int pos = 0; pos <= 180; pos += 5) {
    horizontalServo.write(pos);
    delay(15);
  }

  digitalWrite(greenLedAPin, HIGH);
  digitalWrite(greenLedBPin, HIGH);
  delay(1000);
  turnOffLeds();
}

void loop() {
  BP32.update();

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    GamepadPtr gamepad = gamepadList[i];

    if (gamepad && gamepad->isConnected()) {
      turnOffLeds();
      horizontalAction(gamepad);
      printDebug(gamepad);

      // Executa rotinas com base no botão pressionado
      if (gamepad->y()) yButtonAction();
      if (gamepad->b()) bButtonAction();
      if (gamepad->a()) aButtonAction();
      if (gamepad->x()) xButtonAction();
      if (gamepad->l1()) l1ButtonAction();
      if (gamepad->r2()) r1ButtonAction();
    }
  }
  delay(150);
}
