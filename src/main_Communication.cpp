#include "main_Config.h"

SemaphoreHandle_t SemaphoreMesureEau;
double EauFrequence = 0;
double EauPhase = 0;
double EauTempsMesure = 0;

SemaphoreHandle_t SemaphoreMesureAngle;
double AngleFrequence = 0;
double AnglePhase = 0;
double AngleTempsMesure = 0;

SemaphoreHandle_t SemaphoreInstruction;
double OuvertureVanne = 0; // 0 = ferme, 1 = ouvert

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

bool GetEauData(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut)
{
    if(xSemaphoreTake(SemaphoreMesureEau, WaitPeriod) == pdTRUE)
    {
        FrequenceOut = EauFrequence;
        PhaseOut = EauPhase;
        TempsMesureOut = EauTempsMesure;
        xSemaphoreGive(SemaphoreMesureEau);
        return true;
    }
    return false;
}

bool GetEauDataNonBlocking(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut)
{
    if(xSemaphoreTake(SemaphoreMesureEau, 0) == pdTRUE)
    {
        FrequenceOut = EauFrequence;
        PhaseOut = EauPhase;
        TempsMesureOut = EauTempsMesure;
        xSemaphoreGive(SemaphoreMesureEau);
        return true;
    }
    return false;
}

void SetEauData(double NewFrequence, double NewPhase, double NewTempsMesure)
{
    xSemaphoreTake(SemaphoreMesureEau, WaitPeriod);
    EauFrequence = NewFrequence;
    EauPhase = NewPhase;
    EauTempsMesure = NewTempsMesure;
    xSemaphoreGive(SemaphoreMesureEau);
}

bool GetAngleData(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut)
{
    if(xSemaphoreTake(SemaphoreMesureAngle, WaitPeriod) == pdTRUE)
    {
        FrequenceOut = AngleFrequence;
        PhaseOut = AnglePhase;
        TempsMesureOut = AngleTempsMesure;
        xSemaphoreGive(SemaphoreMesureAngle);
        return true;
    }
    return false;
}

bool GetAngleDataNonBlocking(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut)
{
    if(xSemaphoreTake(SemaphoreMesureAngle, 0) == pdTRUE)
    {
        FrequenceOut = AngleFrequence;
        PhaseOut = AnglePhase;
        TempsMesureOut = AngleTempsMesure;
        xSemaphoreGive(SemaphoreMesureAngle);
        return true;
    }
    return false;
}

void SetAngleData(double NewFrequence, double NewPhase, double NewTempsMesure)
{
    xSemaphoreTake(SemaphoreMesureAngle, WaitPeriod);
    AngleFrequence = NewFrequence;
    AnglePhase = NewPhase;
    AngleTempsMesure = NewTempsMesure;
    xSemaphoreGive(SemaphoreMesureAngle);
}

double GetOuvertureVanne()
{
    xSemaphoreTake(SemaphoreInstruction, WaitPeriod);
    double tempOuverture = OuvertureVanne;
    xSemaphoreGive(SemaphoreInstruction);
    return tempOuverture;
}

bool GetOuvertureVanneNonBlocking(double& OuvertureVanneOut)
{
    if(xSemaphoreTake(SemaphoreInstruction, 0))
    {
        OuvertureVanneOut = OuvertureVanne;
        xSemaphoreGive(SemaphoreInstruction);
        return true;
    }
    return false;
}

void SetOuvertureVanne(double NewValue)
{
    xSemaphoreTake(SemaphoreInstruction, WaitPeriod);
    OuvertureVanne = Clamp<double>(NewValue, 0, 1);
    xSemaphoreGive(SemaphoreInstruction);
}