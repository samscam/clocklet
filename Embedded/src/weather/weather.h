#pragma once


enum PrecipType {
	Drizzle, Rain, Sleet, Snow
};

struct Weather {
  char synopsis[255];
  int type; // The raw type identifier which is used internally by metoffice
  float precipChance;
	float precipIntensity;
  PrecipType precipType;
  float maxTmp;
  float minTmp;
  float currentTmp;
	bool thunder;
	float windSpeed;
	float cloudCover;
  float pressure;
  bool rainbows;

  inline
  Weather& operator+=(const Weather rhs){
    // strncpy(rhs.synopsis, synopsis, 255);
    precipChance = max(precipChance, rhs.precipChance);
    precipIntensity = max(precipIntensity, rhs.precipIntensity);
    precipType = max(precipType, rhs.precipType);
    maxTmp = max(maxTmp, rhs.maxTmp);
    minTmp = min(minTmp, rhs.minTmp);
    currentTmp = max(currentTmp, rhs.currentTmp);
    
    maxTmp = max(rhs.currentTmp, maxTmp);
    minTmp = min(rhs.currentTmp, minTmp);
    

    thunder = max(thunder, rhs.thunder);
    windSpeed = max(windSpeed, rhs.windSpeed);
    cloudCover = max(cloudCover, rhs.cloudCover);
    pressure = max(pressure, rhs.currentTmp);
    return *this;
  }
};

static const Weather defaultWeather = {
  {.synopsis = "No weather yet"},
  .type = 0,
  .precipChance = 0.0f,
  .precipIntensity = 0.0f,
  .precipType = Rain,
  .maxTmp = -20.0f,
  .minTmp = 50.0f,
  .currentTmp = -20.0f,
  .thunder = false,
  .windSpeed = 0.0f,
  .cloudCover = 0.0f,
  .pressure = 0.0f,
};
