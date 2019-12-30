void Communication_Setup();

float GetNiveauEau();
bool GetNiveauEauNonBlocking(float &NiveauEauOut);
void SetNiveauEau(float NewValue);

bool GetGyroData(float &FrequenceOut, float& PhaseOut);
bool GetGyroDataNonBlocking(float &FrequenceOut, float& PhaseOut);
void SetGyroData(float NewFrequence, float NewPhase);

float GetOuvertureVanne();
bool GetOuvertureVanneNonBlocking(float& OuvertureVanneOut);
void SetOuvertureVanne(float NewValue);