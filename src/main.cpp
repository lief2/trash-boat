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
    NiveauEau = ChangementNiveau / PinsMesuresLen;
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SemaphoreMesure = xSemaphoreCreateBinary();
  xSemaphoreGive(SemaphoreMesure);
  xTaskCreatePinnedToCore(mesures, "Mesures", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(asservissement, "Asservissement", 10000, NULL, 1, NULL, 1);
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}