#include "math_Phase.h"
#include "Arduino.h"
#include "main_config.h"



math_Phase::math_Phase(/* args */)
{
    MaxValue = -infinity(), MinValue = +infinity();
}

math_Phase::~math_Phase()
{
}

void math_Phase::AddDataPoint(double value, double time)
{
    LastRegisteredValueIndex++;
    int index = LastRegisteredValueIndex%ArrayLength; //on enregistre en boucle
    Values[index] = value;
    Times[index] = time;
    if(value > MaxValue)
    {
        MaxValue = value;
    }
    if (value < MinValue)
    {
        MinValue = value;
    }
}

bool math_Phase::GetZeroCrossings(double* Positive, double* Negative)
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

double math_Phase::GetFrequency(double* Positive, double* Negative)
{
    int PLen = sizeof(Positive) / sizeof(double);
    int NLen = sizeof(Negative) / sizeof(double);
    double PPeriod=infinity(), NPeriod=infinity();
    //On prend la periode entre les differents passages a zero, sur la plus grande duree possible
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

double math_Phase::GetPhase(double* Positive, double* Negative, double Frequency, double time)
{
    //int PLen = sizeof(Positive) / sizeof(double);
    //int NLen = sizeof(Negative) / sizeof(double);
    double timelastpositive = Positive[0], timelastnegative = Negative[0];
    //Au temps de la moyenne entre un passage 0+ et 0-, on est sur un pic
    //Comme on connait la frequence et la phase au niveau de ce pic, on peut en deduire la phase
    double PeakTime = (timelastpositive + timelastnegative)/2;
    double PhaseOffset = 0;
    if(timelastpositive > timelastnegative) //Pic negatif
    {
        //sin(PhaseOffset) = -1
        PhaseOffset = -PI/2;
    }
    else //Pic positif
    {
        //sin(PhaseOffset) = 1
        PhaseOffset = PI/2;
    }
    double Phase = fmod(PhaseOffset + (time - PeakTime) * TWO_PI * Frequency, TWO_PI);
    return Phase;
}

bool math_Phase::GetFrequencyPhaseAmplitude(double Time, double& Frequency, double& Phase, double& Amplitude)
{
    double *Positive, *Negative;
    if(!GetZeroCrossings(Positive, Negative))
    {
        return false;
    }
    Frequency = GetFrequency(Positive, Negative);
    Phase = GetPhase(Positive, Negative, Frequency, Time);
    Amplitude = GetAmplitude();
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