// int ldr = 13;
int ldrPin = 13;
int ldr;
float temperatura;

void setup() {
  Serial.begin(115200); // Inicia a porta serial
  analogReadResolution(12); // resolução do ADC 12 bits
  analogSetAttenuation(ADC_11db);
}

void loop() {
  ldr = analogRead(ldrPin);
  Serial.print("LDR é: ");
  Serial.println(ldr);
  delay(1000);
}
