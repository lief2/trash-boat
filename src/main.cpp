#include <Arduino.h>

template<class T>
T Clamp(T x, T minimum, T maximum)
{
  return max(min(x, maximum), minimum);
}

const int PinsMesures[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 15, 2, 4};
const int PinsMesuresLen = sizeof(PinsMesures) / sizeof(int);

SemaphoreHandle_t SemaphoreMesure;
float NiveauEau = 0;

void mesures(void* parameters)
{
  int Mesures[PinsMesuresLen], deltas[PinsMesuresLen-1];
  for (;;)
  {
    int PlusGrand = 0, SecondPlusGrand = 0;
    for (int i = 0; i < PinsMesuresLen; i++)
    {
      Mesures[i] = analogRead(PinsMesures[i]);
    }
    for (int i = 0; i < PinsMesuresLen - 1; i++)
    {
      deltas[i] = abs(Mesures[i + 1] - Mesures[i]);
    }
    for (int i = 0; i < PinsMesuresLen - 1; i++)
    {
      if(deltas[i] > deltas[PlusGrand])
      {
        SecondPlusGrand = PlusGrand;
        PlusGrand = i;
      }
      else if (deltas[i] > deltas[SecondPlusGrand])
      {
        SecondPlusGrand = i;
      }
    }
    float ChangementNiveau = deltas[SecondPlusGrand] / (2.f * deltas[PlusGrand]) * (SecondPlusGrand - PlusGrand) + PlusGrand;
    //Serial.printf("Changement de niveau a %f \r\n", ChangementNiveau);
    xSemaphoreTake(SemaphoreMesure, 100/portTICK_PERIOD_MS);
    NiveauEau = ChangementNiveau / (PinsMesuresLen - 1);
    xSemaphoreGive(SemaphoreMesure);
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

const float Kp = 1, Ki = 1, Kd = 1;
const float AbsIMax = 1;

void asservissement(void* parameters)
{
  float cible = 0, erreur = 0, erreurprecedente = 0, P = 0, I = 0, D = 0, consigne = 0;
  long long unsigned int microsprecedente = micros();
  for (;;)
  {
    xSemaphoreTake(SemaphoreMesure, 100/portTICK_PERIOD_MS);
    erreur = cible - NiveauEau;
    xSemaphoreGive(SemaphoreMesure);
    long long unsigned int deltamicros = micros() - microsprecedente;
    microsprecedente = micros();
    P = erreur * Kp;
    I = Clamp<float>(I + Ki * erreur * deltamicros / 1e6, -AbsIMax, AbsIMax);
    D = Kd * (erreur - erreurprecedente) * 1e6 / deltamicros;
    consigne = P + I + D;
    erreurprecedente = erreur;
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

#include <TMCStepper.h>
#define EN_PIN           5 // Enable
#define DIR_PIN          19 // Direction
#define STEP_PIN         18 // Step
#define SERIAL_PORT Serial2 // TMC2208/TMC2224 HardwareSerial port
#define R_SENSE 0.11f
TMC2208Stepper driver(&SERIAL_PORT, R_SENSE);

#define STEP_PER_REV 200
#define MICROSTEPS 16
#define STEPS_PER_TURN STEP_PER_REV * MICROSTEPS
#define SPEED 2.f
#define STEPS_PER_SECOND SPEED * STEPS_PER_TURN
#define STEP_INTERVAL  1e6 / STEPS_PER_SECOND


void moteur(void* parameters)
{
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  SERIAL_PORT.begin(115200);

  driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.rms_current(400);        // Set motor RMS current
  driver.microsteps(MICROSTEPS);          // Set microsteps to 1/16th

  driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
  driver.pwm_autoscale(true);     // Needed for stealthChop
  
  bool shaft = false;

  for (;;)
  {
    // Run 5000 steps and switch direction in software
    for (uint16_t i = STEPS_PER_TURN /2; i>0; i--) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(STEP_INTERVAL/2);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(STEP_INTERVAL/2);
    }
    shaft = !shaft;
    driver.shaft(shaft);
    //digitalWrite(DIR_PIN, shaft);
  }
  
  vTaskDelete(NULL);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SemaphoreMesure = xSemaphoreCreateBinary();
  xSemaphoreGive(SemaphoreMesure);
  xTaskCreatePinnedToCore(mesures, "Mesures", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(asservissement, "Asservissement", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(moteur, "Moteur", 10000, NULL, 1, NULL, 1);
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}