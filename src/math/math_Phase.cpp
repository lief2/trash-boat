#include "math_Phase.h"
#include "Arduino.h"
#include "main_config.h"



math_Phase::math_Phase(/* args */)
{
}

math_Phase::~math_Phase()
{
}

void math_Phase::AddDataPoint(double value, double time)
{
    LastRegisteredValueIndex++;
    int index = LastRegisteredValueIndex%ArrayLength;
    Values[index] = value;
    Times[index] = time;
    SumOfAllValues += value;
}

void math_Phase::GetZeroCrossings(double* Positive, double* Negative)
{
    Positive = new double[0];
    int PLen = 0;
    Negative = new double[0];
    int NLen = 0;
    int lastindex = 0;
    for (int i = 0; i < ArrayLength; i++)//Itère du dernier sample ajouté au premier
    {
        int index = (LastRegisteredValueIndex-i)%ArrayLength;
        if (index<0)
        {
            break;
        }
        if(i!=0)
        {
            double value = GetValueRelative(index), lastvalue = GetValueRelative(lastindex);
            double time = Times[index], lasttime = Times[lastindex];
            if (value * lastvalue < 0)
            {
                double intercept = Map(0.0, value, lastvalue, time, lasttime);
                if (value > 0)//Negatif
                {
                    NLen++;
                    delete [] Negative;
                    Negative = new double [NLen];
                    Negative[NLen-1] = intercept;
                }
                else //Positif
                {
                    PLen++;
                    delete [] Positive;
                    Positive = new double [PLen];
                    Positive[PLen-1] = intercept;
                }
            }
            
        }
        lastindex = index;
    }
    
}

double math_Phase::GetFrequency()
{
    double *Positive, *Negative;
    GetZeroCrossings(Positive, Negative);
    int PLen = sizeof(Positive) / sizeof(double);
    int NLen = sizeof(Negative) / sizeof(double);
    double PPeriod, NPeriod;
    if (PLen >= 2)
    {
        PPeriod = (Positive[0] - Positive[PLen]) / (PLen-1);
    }
    if (NLen >= 2)
    {
        NPeriod = (Negative[0] - Negative[NLen]) / (NLen-1);
    }
    double Period = (PPeriod + NPeriod)/2;
    return 1/Period;
}


bool GetSinusProperties(double x, double dx, double d2x, double& Amplitude, double& Omega, double& Phase)
{
    double omegasquared = -d2x/x;
    Omega = sqrt(omegasquared);
    Phase = atan2(dx/Omega, x);
    Amplitude = x/sin(Phase);
    return true;
}

double GetDeltaPhase(double reference, double relative)
{
    double delta = relative-reference;
    if (fabs(delta)>PI)
    {
        delta -= copysign(TWO_PI, delta); //Copies sign of delta to 360 then removes it from delta to clamp between 
    }
    return delta;
}