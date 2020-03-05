#include "main_Superviseur.h"
#include "main_Config.h"

#include "main_Moteur.h"

long long lastTimeRunPressed = -DebounceDelay;
bool MoteurRunning = false;
TaskHandle_t MoteurTask;
void RunInterrupt()
{
    if(millis()-lastTimeRunPressed > DebounceDelay)
    {
        lastTimeRunPressed = millis();
        if(!MoteurRunning)
        {
            MoteurRunning = true;
            xTaskCreatePinnedToCore(Moteur_Task, "Moteur", 10000, NULL, 1, &MoteurTask, 1);
        }
        else
        {
            MoteurRunning = false;
            vTaskDelete(MoteurTask);
        }
        digitalWrite(Pin_LED, MoteurRunning);
    }
}

void Superviseur_Task(void* Parameters)
{
    pinMode(Pin_LED, OUTPUT);
    pinMode(Pin_Run, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Pin_Run), RunInterrupt, FALLING);
    
    vTaskDelete(NULL);
}