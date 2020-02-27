#include "math_Derivation.h"

double GetFirstDerivative(double x1, double t1, double x2, double t2)
{
    return (x1-x2)/(t1-t2);
}

double GetSecondDerivative(double x1, double t1, double x2, double t2, double x3, double t3)
{
    double dx1 = GetFirstDerivative(x1,t1,x2,t2);
    double dx2 = GetFirstDerivative(x2,t2,x3,t3);
    return GetFirstDerivative(dx1,(t1+t2)/2,dx2,(t2+t3)/2);
}