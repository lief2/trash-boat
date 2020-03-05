#include "main_Asservissement.h"
#include "main_Config.h"
#include "main_Communication.h"

#include "math/math_Phase.h"

void Asservissement_Task(void* parameters)
{
    double erreur = 0, erreurprecedente = 0, P = 0, I = 0, D = 0, consigne = 0;
    long long unsigned int microsprecedente = micros();
    double FrequenceAngle, PhaseAngle, TempsAngle, FrequenceEau, PhaseEau, TempsEau;
    GetAngleData(FrequenceAngle, PhaseAngle, TempsAngle);
    GetEauData(FrequenceEau, PhaseEau, TempsEau);
    for (;;)
    {
        GetAngleDataNonBlocking(FrequenceAngle, PhaseAngle, TempsAngle);
        double TempsActuel = ((float)millis())/1000.0;
        double PhaseActuelleAngle = PhaseAngle + FrequenceAngle * (TempsActuel-TempsAngle);
        double PhaseActuelleEau = PhaseEau + FrequenceEau * (TempsActuel-TempsAngle);
        erreur = cibleDecalage - GetDeltaPhase(PhaseActuelleAngle, PhaseActuelleEau);
        long long unsigned int deltamicros = micros() - microsprecedente;
        microsprecedente = micros();
        P = erreur * Kp;
        I = Clamp<double>(I + Ki * erreur * deltamicros / 1e6, -AbsIMax, AbsIMax);
        D = Kd * (erreur - erreurprecedente) * 1e6 / deltamicros;
        consigne = P + I + D;
        //SetOuvertureVanne(consigne);
        erreurprecedente = erreur;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}