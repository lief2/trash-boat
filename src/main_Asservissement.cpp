#include "main_Asservissement.h"
#include "main_Config.h"
#include "main_Communication.h"

const float Kp = 1, Ki = 1, Kd = 1;
const float AbsIMax = 1;

void Asservissement_Task(void* parameters)
{
    float cible = 0, erreur = 0, erreurprecedente = 0, P = 0, I = 0, D = 0, consigne = 0;
    long long unsigned int microsprecedente = micros();
    for (;;)
    {
        erreur = cible - GetNiveauEau();
        long long unsigned int deltamicros = micros() - microsprecedente;
        microsprecedente = micros();
        P = erreur * Kp;
        I = Clamp<float>(I + Ki * erreur * deltamicros / 1e6, -AbsIMax, AbsIMax);
        D = Kd * (erreur - erreurprecedente) * 1e6 / deltamicros;
        consigne = P + I + D;
        //SetOuvertureVanne(consigne);
        erreurprecedente = erreur;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}