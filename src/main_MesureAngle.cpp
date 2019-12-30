#include "main_MesureAngle.h"
#include "main_Config.h"
#include "main_Communication.h"

#include <MPU6050_tockn.h>
#include <Wire.h>
#include "arduinoFFT.h"

#define SDA 21
#define SCL 22

MPU6050 mpu6050(Wire);

/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 250; //Hz
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];
int currentIndex = 0;
long long unsigned int MeasureMicros = 0;
unsigned int MicrosPeriod = 1e6/samplingFrequency;

arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);

void MesureAngle_Task(void* Parameters)
{
    Wire.begin(SDA, SCL, 400000L);
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true, 0, 0);
    for (;;)
    {
        delayMicroseconds(MeasureMicros + MicrosPeriod - micros());
        mpu6050.update();
        MeasureMicros += MicrosPeriod;
        float AngleDeg = mpu6050.getAngleX();
        if(currentIndex < samples)
        {
            vReal[currentIndex] = AngleDeg;
            vImag[currentIndex] = 0.0;
            currentIndex++;
        }
        else
        {
            for (int i = 0; i < samples-1; i++)
            {
            vReal[i] = vReal[i+1];
            vImag[i] = 0.0;
            }
            vReal[samples-1] = AngleDeg;
            vImag[samples-1] = 0.0;
        }
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(FFT_FORWARD);
        double RollFrequency, RollAmplitude; 
        FFT.MajorPeak(&RollFrequency, &RollAmplitude);
        //vTaskDelay(1);
    }

    vTaskDelete(NULL);
}