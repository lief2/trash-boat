/*
x is the value
dx is the first derivative of the value
d2x is the second derivative of the value
Function of x is expected to be of the form x(t) = Amplitude * sin(Phase), and omega is the rotation speed
Phase is given by atan2 and is in radians, Omega is in radians/s
dx = A*omega*cos(Phase)
d2x = -A*omega^2*sin(Phase)
*/
bool GetSinusProperties(double x, double dx, double d2x, double& Amplitude, double& Omega, double& Phase);

double GetDeltaPhase(double reference, double relative);