#include <Arduino.h>

template<class T>
T Clamp(T x, T minimum, T maximum)
{
  return max(min(x, maximum), minimum);
}

template<class T>
T Lerp(T alpha, T a, T b)
{
  return a + alpha * (b-a);
}

namespace Communication
{
  SemaphoreHandle_t SemaphoreMesure;
  float NiveauEau = 0;

  SemaphoreHandle_t SemaphoreInstruction;
  float OuvertureVanne = 0; // 0 = ferme, 1 = ouvert

  const int WaitPeriod = 100/portTICK_PERIOD_MS;

  void InitSemaphores()
  {
    SemaphoreMesure = xSemaphoreCreateBinary();
    xSemaphoreGive(SemaphoreMesure);
    SemaphoreInstruction = xSemaphoreCreateBinary();
    xSemaphoreGive(SemaphoreInstruction);
  }

  float GetNiveauEau()
  {
    xSemaphoreTake(SemaphoreMesure, WaitPeriod);
    float tempNiveau = NiveauEau;
    xSemaphoreGive(SemaphoreMesure);
    return tempNiveau;
  }

  void SetNiveauEau(float NewValue)
  {
    xSemaphoreTake(SemaphoreMesure, WaitPeriod);
    NiveauEau = NewValue;
    xSemaphoreGive(SemaphoreMesure);
  }

  float GetOuvertureVanne()
  {
    xSemaphoreTake(SemaphoreMesure, WaitPeriod);
    float tempOuverture = OuvertureVanne;
    xSemaphoreGive(SemaphoreMesure);
    return tempOuverture;
  }

  void SetOuvertureVanne(float NewValue)
  {
    xSemaphoreTake(SemaphoreInstruction, WaitPeriod);
    OuvertureVanne = NewValue;
    xSemaphoreGive(SemaphoreInstruction);
  }
}

namespace MesureEau{
  const int Pins[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 15, 2, 4};
  const int PinsLen = sizeof(Pins) / sizeof(int);
  void Task(void* parameters)
  {
    int Mesures[PinsLen], deltas[PinsLen-1];
    for (;;)
    {
      int PlusGrand = 0, SecondPlusGrand = 0;
      for (int i = 0; i < PinsLen; i++)
      {
        Mesures[i] = analogRead(Pins[i]);
      }
      for (int i = 0; i < PinsLen - 1; i++)
      {
        deltas[i] = abs(Mesures[i + 1] - Mesures[i]);
      }
      for (int i = 0; i < PinsLen - 1; i++)
      {
        if(deltas[i] > deltas[PlusGrand])
        {
          SecondPlusGrand = PlusGrand;
          PlusGrand = i;
        }
        else if (deltas[i] > deltas[SecondPlusGrand])
        {
          SecondPlusGrand = i;
        }
      }
      float ChangementNiveau = deltas[SecondPlusGrand] / (2.f * deltas[PlusGrand]) * (SecondPlusGrand - PlusGrand) + PlusGrand;
      //Serial.printf("Changement de niveau a %f \r\n", ChangementNiveau);
      Communication::SetNiveauEau(ChangementNiveau / (PinsLen - 1));
      vTaskDelay(1);
    }
    vTaskDelete(NULL);
  }
}

#include <MPU6050_tockn.h>
#include <Wire.h>
#include "arduinoFFT.h"
namespace MesureAngle{
  #define SDA 21
  #define SCL 22

  MPU6050 mpu6050(Wire);

  

  /*
  These values can be changed in order to evaluate the functions
  */
  const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
  const double samplingFrequency = 250; //Hz
  /*
  These are the input and output vectors
  Input vectors receive computed results from FFT
  */
  double vReal[samples];
  double vImag[samples];
  int currentIndex = 0;
  long long unsigned int MeasureMicros = 0;
  unsigned int MicrosPeriod = 1e6/samplingFrequency;

  arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);

  void Task(void* Parameters)
  {
    Wire.begin(SDA, SCL, 400000L);
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true, 0, 0);
    for (;;)
    {
      delayMicroseconds(MeasureMicros + MicrosPeriod - micros());
      mpu6050.update();
      MeasureMicros += MicrosPeriod;
      float AngleDeg = mpu6050.getAngleX();
      if(currentIndex < samples)
      {
        vReal[currentIndex] = AngleDeg;
        vImag[currentIndex] = 0.0;
        currentIndex++;
      }
      else
      {
        for (int i = 1; i < samples; i++)
        {
          vReal[i-1] = vReal[i];
          vImag[i-1] = 0.0;
        }
        vReal[currentIndex] = AngleDeg;
        vImag[currentIndex] = 0.0;
      }
      FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(FFT_FORWARD);
      double RollFrequency, RollAmplitude; 
      FFT.MajorPeak(&RollFrequency, &RollAmplitude);
      //vTaskDelay(1);
    }
    
    vTaskDelete(NULL);
  }
}

namespace Asservissement{
  const float Kp = 1, Ki = 1, Kd = 1;
  const float AbsIMax = 1;

  void Task(void* parameters)
  {
    float cible = 0, erreur = 0, erreurprecedente = 0, P = 0, I = 0, D = 0, consigne = 0;
    long long unsigned int microsprecedente = micros();
    for (;;)
    {
      erreur = cible - Communication::GetNiveauEau();
      long long unsigned int deltamicros = micros() - microsprecedente;
      microsprecedente = micros();
      P = erreur * Kp;
      I = Clamp<float>(I + Ki * erreur * deltamicros / 1e6, -AbsIMax, AbsIMax);
      D = Kd * (erreur - erreurprecedente) * 1e6 / deltamicros;
      consigne = P + I + D;
      //Communication::SetOuvertureVanne(consigne);
      erreurprecedente = erreur;
      vTaskDelay(1);
    }
    vTaskDelete(NULL);
  }
}

#include <TMCStepper.h>
#include <AccelStepper.h>
namespace Stepper{
  #define EN_PIN           5 // Enable
  #define DIR_PIN          19 // Direction
  #define STEP_PIN         18 // Step
  #define SERIAL_PORT Serial2 // TMC2208/TMC2224 HardwareSerial port
  #define R_SENSE 0.11f
  AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);
  TMC2208Stepper driver(&Serial2, R_SENSE);

  const int STEP_PER_REV = 200;
  const int MICROSTEPS = 16;
  const int STEPS_PER_TURN = STEP_PER_REV * MICROSTEPS;
  const float SPEED = 2.f; //tours/s
  const float STEPS_PER_SECOND = STEPS_PER_TURN * SPEED;
  const int STEP_INTERVAL_US = 1e6 / STEPS_PER_SECOND;
  const float OPEN_ROT = 0; //tours
  const float CLOSED_ROT = 1.5f; //tours

  void DriverSetup()
  {
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);
    //UART
    SERIAL_PORT.begin(115200);

    driver.begin();                 // Initialisation UART
    driver.toff(5);                 // Active le controlleur
    driver.rms_current(400);        // Courant moteur (mA)
    driver.microsteps(16);  // Microsteps

    //driver.en_spreadCycle(false);   // SpreadCycle (plus de bruit, plus de couple que StealthChop)
    driver.pwm_autoscale(true);     // Requis pour StealthChop

    Serial.println("UART Initialised");
  }

  void Task(void* parameters)
  {
    //Initialisation du moteur
    /*stepper.setMaxSpeed(STEPS_PER_SECOND); // 2 tours/s @ 6400 uSteps/s
    stepper.setAcceleration(20*STEPS_PER_SECOND); // 40 tours/s/s
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();*/
  
    Serial.println("Stepper Initialised");
    vTaskDelay(1000/portTICK_PERIOD_MS);

    //Boucle
    for (;;)
    {
      /*float OuvertureVanne = abs(cos((millis() * 2 * PI) / 1000)); //Communication::GetOuvertureVanne();
      OuvertureVanne = Clamp<float>(OuvertureVanne, 0, 1); //0-1
      float Position = Lerp<float>(OuvertureVanne, CLOSED_ROT, OPEN_ROT); //tours
      int steps = Position * STEPS_PER_TURN;*/
      int open_steps = STEPS_PER_TURN * OPEN_ROT;
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
      }
      //vTaskDelay(1);
    }

    vTaskDelete(NULL);
  }
}
void setup() {
  Serial.begin(115200);
  Communication::InitSemaphores();
  Stepper::DriverSetup();
  xTaskCreatePinnedToCore(MesureEau::Task, "Mesures du niveau d'eau", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(MesureAngle::Task, "Mesure de l'angle", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Asservissement::Task, "Asservissement", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Stepper::Task, "Moteur", 10000, NULL, 1, NULL, 1);
  vTaskDelete(NULL);
}

void loop() {
  //Inutile car tout le code tourne deja dans les differentes taches
}