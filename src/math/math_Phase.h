class math_Phase
{
private:
	const int ArrayLength = 256;
    double* Values = new double[ArrayLength];
	double* Times = new double[ArrayLength];
	double MaxValue, MinValue;
	int LastRegisteredValueIndex = -1;
public:
    math_Phase(/* args */);
    ~math_Phase();
	void AddDataPoint(double value, double time);
	double GetValueRelative(int index)
	{
		return Values[index] - (MaxValue+MinValue)/2.0;
	}
	bool GetZeroCrossings(double* Positive, double* Negative); //les evenements les plus recents sont en premier
	double GetFrequency(double* Positive, double* Negative);
	double GetPhase(double* Positive, double* Negative, double Frequency, double time);
	double GetAmplitude()
	{
		return (MaxValue - MinValue)/2.0;
	}
	bool GetFrequencyPhaseAmplitude(double Time, double& Frequency, double& Phase, double& Amplitude);
};

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