#include "main_MesureAngle.h"
#include "main_Config.h"
#include "main_Communication.h"

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

    vTaskDelete(NULL);
}