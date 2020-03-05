#include "main_Moteur.h"
#include "main_Config.h"
#include "main_Communication.h"

#include <AccelStepper.h>

AccelStepper stepper = AccelStepper(stepper.DRIVER, Pin_Step_Moteur, Pin_Direction_Moteur);

void Moteur_Setup()
{
    pinMode(Pin_Step_Moteur, OUTPUT);
    pinMode(Pin_Direction_Moteur, OUTPUT);
    pinMode(Pin_Enable_Moteur, OUTPUT);
    digitalWrite(Pin_Enable_Moteur, LOW);
}

void Moteur_Task(void* parameters)
{
    //Initialisation du moteur
    stepper.setMaxSpeed(ImpulsionsParSeconde_Moteur); // 2 tours/s @ 6400 uSteps/s
    stepper.setAcceleration(20*ImpulsionsParSeconde_Moteur); // 40 tours/s/s
    stepper.setEnablePin(Pin_Enable_Moteur);
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
        double OuvertureVanne;
        if(GetOuvertureVanneNonBlocking(OuvertureVanne))
        {
            float Position = Lerp<float>(OuvertureVanne, RotationFerme_Moteur, RotationOuvert_Moteur);
            int steps = Position * MicroimpulsionsParTourMoteur;
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
        /*int open_steps = MicroimpulsionsPaRotationOuvert_Moteureur * OPEN_ROT;
        int closed_steps = MicroimpulsionsPaRotationFerme_Moteurr * CLOSED_ROT;
        digitalWrite(Pin_Direction_Moteur, HIGH);
        for (int i = 0; i < closed_steps; i++)
        {
        digitalWrite(Pin_Step_Moteur, HIGH);
        delayMicroseconds(IntervalImpulsion_Moteur/2);
        digitalWrite(Pin_Step_Moteur, LOW);
        delayMicroseconds(IntervalImpulsion_Moteur/2);
        }
        digitalWrite(Pin_Direction_Moteur, LOW);
        for (int i = 0; i < closed_steps; i++)
        {
        digitalWrite(Pin_Step_Moteur, HIGH);
        delayMicroseconds(IntervalImpulsion_Moteur/2);
        digitalWrite(Pin_Step_Moteur, LOW);
        delayMicroseconds(IntervalImpulsion_Moteur/2);
        }*/
        //vTaskDelay(1);
    }

    vTaskDelete(NULL);
}