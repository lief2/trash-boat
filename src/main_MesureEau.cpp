#include "main_MesureEau.h"
#include "main_Config.h"
#include "main_Communication.h"

const int Pins[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 15, 2, 4};
const int PinsLen = sizeof(Pins) / sizeof(int);
void MesureEau_Task(void* parameters)
{
int Mesures[PinsLen], deltas[PinsLen-1];
for (;;)
{
    int PlusGrand = 0, SecondPlusGrand = 0;
    for (int i = 0; i < PinsLen; i++)
    {
    Mesures[i] = analogRead(Pins[i]);
    }
    for (int i = 0; i < PinsLen - 1; i++)
    {
    deltas[i] = abs(Mesures[i + 1] - Mesures[i]);
    }
    for (int i = 0; i < PinsLen - 1; i++)
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
    SetNiveauEau(ChangementNiveau / (PinsLen - 1));
    vTaskDelay(1);
}
vTaskDelete(NULL);
}