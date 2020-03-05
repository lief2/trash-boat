#include <Arduino.h>

//Boutons et led
const int DebounceDelay = 200;//ms
const int Pin_Calibrate = 16, Pin_Run = 17;
const int Pin_LED = 23;

//Mesure Eau
const int PinsMesureEau[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 15, 2, 4};
const int PinsMesureEauLen = sizeof(PinsMesureEau) / sizeof(int);

//Moteur pas a pas
const int Pin_Enable_Moteur = 5; // Enable
const int Pin_Direction_Moteur = 19; // Direction
const int Pin_Step_Moteur = 18; // Step

const int ImpulsionsParTour_Moteur = 200;
const int Microsteps_Moteur = 16;
const int MicroimpulsionsParTourMoteur = ImpulsionsParTour_Moteur * Microsteps_Moteur;
const float Vitesse_Moteur = 2.f; //tours/s
const float ImpulsionsParSeconde_Moteur = MicroimpulsionsParTourMoteur * Vitesse_Moteur;
const int IntervalImpulsion_Moteur = 1e6 / ImpulsionsParSeconde_Moteur;
const float RotationOuvert_Moteur = 0; //tours
const float RotationFerme_Moteur = 1.5f; //tours

//Asservissement
const float Kp = 1, Ki = 1, Kd = 1;
const float AbsIMax = 1;
const float cibleDecalage = -PI/2;

//Fonctions generiques que Arduino n'a pas mais devrait avoir
template<class T>
T Clamp(T x, T minimum, T maximum)
{
  return max(min(x, maximum), minimum);
}

template<class T>
T Lerp(T alpha, T a, T b)
{
  return a + alpha * (b-a);
}