#include "main_MesureEau.h"
#include "main_Config.h"
#include "main_Communication.h"

#include "math/math_Derivation.h"
#include "math/math_Phase.h"

math_Phase PhaseEau;

void MesureEau_Task(void* parameters)
{
    for (int i = 0; i < PinsMesureEauLen; i++)
    {
        pinMode(PinsMesureEau[i], INPUT);
    }

    for (;;)
    {
        //Faire la mesure

        double time = ((double)micros()) / 1000000.0;
        double angle = 1;
        
        PhaseEau.AddDataPoint(angle, time);
        double Frequency, Phase, Amplitude;
        if(PhaseEau.GetFrequencyPhaseAmplitude(time, Frequency, Phase, Amplitude))
        {
            SetEauData(Frequency, Phase, time);
        }
        delay(100);
    }
    vTaskDelete(NULL);
}