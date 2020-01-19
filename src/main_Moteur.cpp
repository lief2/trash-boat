#include "main_Moteur.h"
#include "main_Config.h"
#include "main_Communication.h"

#include <AccelStepper.h>

#define EN_PIN           5 // Enable
#define DIR_PIN          19 // Direction
#define STEP_PIN         18 // Step
#define SERIAL_PORT Serial2 // TMC2208/TMC2224 HardwareSerial port
#define R_SENSE 0.11f
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

const int STEP_PER_REV = 200;
const int MICROSTEPS = 16;
const int STEPS_PER_TURN = STEP_PER_REV * MICROSTEPS;
const float SPEED = 2.f; //tours/s
const float STEPS_PER_SECOND = STEPS_PER_TURN * SPEED;
const int STEP_INTERVAL_US = 1e6 / STEPS_PER_SECOND;
const float OPEN_ROT = 0; //tours
const float CLOSED_ROT = 1.5f; //tours

void Moteur_Setup()
{
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);
}

void Moteur_Task(void* parameters)
{
    //Initialisation du moteur
    stepper.setMaxSpeed(STEPS_PER_SECOND); // 2 tours/s @ 6400 uSteps/s
    stepper.setAcceleration(20*STEPS_PER_SECOND); // 40 tours/s/s
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();
    stepper.setMinPulseWidth(100);

    Serial.println("Stepper Initialised");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    //long lastreport = millis();
    //long ticks = 0;
    //Boucle
    for (;;)
    {
        float OuvertureVanne;
        if(GetOuvertureVanneNonBlocking(OuvertureVanne))
        {
            float Position = Lerp<float>(OuvertureVanne, CLOSED_ROT, OPEN_ROT);
            int steps = Position * STEPS_PER_TURN;
            stepper.moveTo(steps);
        }
        stepper.run();
        /*ticks++;
        long time = millis()-lastreport;
        if(time >= 60*1000)
        {
            Serial.printf("Done %li ticks in %li millis\r\n", ticks, time);
            lastreport = millis();
            ticks = 0;
        }*/
        /*int open_steps = STEPS_PER_TURN * OPEN_ROT;
        int closed_steps = STEPS_PER_TURN * CLOSED_ROT;
        digitalWrite(DIR_PIN, HIGH);
        for (int i = 0; i < closed_steps; i++)
        {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(STEP_INTERVAL_US/2);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(STEP_INTERVAL_US/2);
        }
        digitalWrite(DIR_PIN, LOW);
        for (int i = 0; i < closed_steps; i++)
        {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(STEP_INTERVAL_US/2);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(STEP_INTERVAL_US/2);
        }*/
        //vTaskDelay(1);
    }

    vTaskDelete(NULL);
}