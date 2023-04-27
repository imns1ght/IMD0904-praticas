int lm35 = 13;
int adc;
float temperatura;

void setup() {
  Serial.begin(115200); // Inicia a porta serial
  analogReadResolution(12); // resolução do ADC 12 bits
  analogSetAttenuation(ADC_0db);
}

void loop() {
  adc = analogRead(lm35);
  temperatura = adc * 0.02686203;
  Serial.print("Temperatura é: ");
  Serial.print(temperatura);
  Serial.println(" C");
  delay(1000);
}
