#include "main_MesureAngle.h"
#include "main_Config.h"
#include "main_Communication.h"

#include "math/math_Derivation.h"
#include "math/math_Phase.h"

#include <MPU6050_tockn.h>
#include <Wire.h>

#define SDA 21
#define SCL 22

MPU6050 mpu6050(Wire);

math_Phase PhaseAngle;

void MesureAngle_Setup()
{
    Wire.begin(SDA, SCL, 400000L);
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true, 0, 0);
}

void MesureAngle_Task(void* Parameters)
{
    for (;;)
    {
        mpu6050.update();
        double time = ((double)micros()) / 1000000.0;
        double angle = mpu6050.getAngleX();
        PhaseAngle.AddDataPoint(angle, time);

        double Frequence, Phase, Amplitude;
        if(PhaseAngle.GetFrequencyPhaseAmplitude(time, Frequence, Phase, Amplitude))
        {
            SetAngleData(Frequence, Phase, time);
        }
        delay(100);
    }
    
    vTaskDelete(NULL);
}