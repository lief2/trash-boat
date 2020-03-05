void Communication_Setup();

bool GetEauData(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut);
bool GetEauDataNonBlocking(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut);
void SetEauData(double NewFrequence, double NewPhase, double NewTempsMesure);

bool GetAngleData(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut);
bool GetAngleDataNonBlocking(double &FrequenceOut, double& PhaseOut, double& TempsMesureOut);
void SetAngleData(double NewFrequence, double NewPhase, double NewTempsMesure);

double GetOuvertureVanne();
bool GetOuvertureVanneNonBlocking(double& OuvertureVanneOut);
void SetOuvertureVanne(double NewValue);