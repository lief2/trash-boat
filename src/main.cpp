#include <Arduino.h>
#include "main_Config.h"
#include "main_Communication.h"
#include "main_Superviseur.h"
#include "main_MesureEau.h"
#include "main_MesureAngle.h"
#include "main_Asservissement.h"
#include "main_Moteur.h"


void setup() {
  Serial.begin(115200);
  Communication_Setup();
  MesureAngle_Setup();
  Moteur_Setup();
  xTaskCreatePinnedToCore(Superviseur_Task, "Superviseur", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(MesureEau_Task, "Mesures du niveau d'eau", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(MesureAngle_Task, "Mesure de l'angle", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Asservissement_Task, "Asservissement", 10000, NULL, 1, NULL, 0);
  vTaskDelete(NULL);
}

void loop() {
  //Inutile car tout le code tourne deja dans les differentes taches
}