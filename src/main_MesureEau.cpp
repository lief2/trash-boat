#include "main_MesureEau.h"
#include "main_Config.h"
#include "main_Communication.h"

#include "math/math_Derivation.h"
#include "math/math_Phase.h"


void MesureEau_Task(void* parameters)
{
    for (int i = 0; i < PinsMesureEauLen; i++)
    {
        pinMode(PinsMesureEau[i], INPUT);
    }

    double x1=0,t1=0,x2=0,t2=0,x3=NAN,t3=0;

    for (;;)
    {
        //Faire la mesure

        double time = ((double)micros()) / 1000000.0;
        double angle = 1;
        x3=x2; t3=t2; x2=x1; t2=t1; x1=angle; t1=time;
        if(!isnan(x3))
        {
            double dx = GetFirstDerivative(x1,t1,x2,t2);
            double d2x = GetSecondDerivative(x1,t1,x2,t2,x3,t3);
            double Amplitude, Omega, Phase;
            if(GetSinusProperties(x1, dx, d2x, Amplitude, Omega, Phase))
            {
                SetEauData(Omega, Phase, (t1+t2+t3)/3);
            }
        }
        delay(100);
    }
    
    
    vTaskDelete(NULL);
}