#include "main_Config.h"

SemaphoreHandle_t SemaphoreMesureEau;
float NiveauEau = 0;

SemaphoreHandle_t SemaphoreMesureAngle;
float Frequence = 0;
float Phase = 0;

SemaphoreHandle_t SemaphoreInstruction;
float OuvertureVanne = 0; // 0 = ferme, 1 = ouvert

const int WaitPeriod = 100/portTICK_PERIOD_MS;

void Communication_Setup()
{
    SemaphoreMesureEau = xSemaphoreCreateBinary();
    xSemaphoreGive(SemaphoreMesureEau);
    SemaphoreMesureAngle = xSemaphoreCreateBinary();
    xSemaphoreGive(SemaphoreMesureAngle);
    SemaphoreInstruction = xSemaphoreCreateBinary();
    xSemaphoreGive(SemaphoreInstruction);
}

float GetNiveauEau()
{
    xSemaphoreTake(SemaphoreMesureEau, WaitPeriod);
    float tempNiveau = NiveauEau;
    xSemaphoreGive(SemaphoreMesureEau);
    return tempNiveau;
}

bool GetNiveauEauNonBlocking(float &NiveauEauOut)
{
    if(xSemaphoreTake(SemaphoreMesureEau, 0) == pdTRUE)
    {
        NiveauEauOut = NiveauEau;
        xSemaphoreGive(SemaphoreMesureEau);
        return true;
    }
    return false;
}

void SetNiveauEau(float NewValue)
{
    xSemaphoreTake(SemaphoreMesureEau, WaitPeriod);
    NiveauEau = NewValue;
    xSemaphoreGive(SemaphoreMesureEau);
}

bool GetGyroData(float &FrequenceOut, float& PhaseOut)
{
    if(xSemaphoreTake(SemaphoreMesureAngle, WaitPeriod) == pdTRUE)
    {
        FrequenceOut = Frequence;
        PhaseOut = Phase;
        xSemaphoreGive(SemaphoreMesureAngle);
        return true;
    }
    return false;
}

bool GetGyroDataNonBlocking(float &FrequenceOut, float& PhaseOut)
{
    if(xSemaphoreTake(SemaphoreMesureAngle, 0) == pdTRUE)
    {
        FrequenceOut = Frequence;
        PhaseOut = Phase;
        xSemaphoreGive(SemaphoreMesureAngle);
        return true;
    }
    return false;
}

void SetGyroData(float NewFrequence, float NewPhase)
{
    xSemaphoreTake(SemaphoreMesureAngle, WaitPeriod);
    Frequence = NewFrequence;
    Phase = NewPhase;
    xSemaphoreGive(SemaphoreMesureAngle);
}

float GetOuvertureVanne()
{
    xSemaphoreTake(SemaphoreInstruction, WaitPeriod);
    float tempOuverture = OuvertureVanne;
    xSemaphoreGive(SemaphoreInstruction);
    return tempOuverture;
}

bool GetOuvertureVanneNonBlocking(float& OuvertureVanneOut)
{
    if(xSemaphoreTake(SemaphoreInstruction, 0))
    {
        OuvertureVanneOut = OuvertureVanne;
        xSemaphoreGive(SemaphoreInstruction);
        return true;
    }
    return false;
}

void SetOuvertureVanne(float NewValue)
{
    xSemaphoreTake(SemaphoreInstruction, WaitPeriod);
    OuvertureVanne = Clamp<float>(NewValue, 0, 1);
    xSemaphoreGive(SemaphoreInstruction);
}