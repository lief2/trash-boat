#include "math_Phase.h"
#include "Arduino.h"

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