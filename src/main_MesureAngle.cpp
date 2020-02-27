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

void MesureAngle_Task(void* Parameters)
{
    Wire.begin(SDA, SCL, 400000L);
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true, 0, 0);
    double x1,t1,x2,t2,x3=NAN,t3;
    for (;;)
    {
        mpu6050.update();
        double time = ((double)micros()) / 1000000.0;
        double angle = mpu6050.getAngleX();
        x3=x2; t3=t2; x2=x1; t2=t1; x1=angle; t1=time;
        if(!isnan(x3))
        {
            double dx = GetFirstDerivative(x1,t1,x2,t2);
            double d2x = GetSecondDerivative(x1,t1,x2,t2,x3,t3);
            double Amplitude, Omega, Phase;
            if(GetSinusProperties(x1, dx, d2x, Amplitude, Omega, Phase))
            {
                SetGyroData(Omega, Phase);
            }
        }
    }
    
    vTaskDelete(NULL);
}