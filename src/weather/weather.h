#ifndef PRISCILLA_WEATHER
#define PRISCILLA_WEATHER


enum PrecipType {
	Rain, Snow, Sleet
};

struct Weather {
  const char* summary;
  int type; // The raw type identifier which is used internally by metoffice
  float precipChance;
	float precipIntensity;
  PrecipType precipType;
  float maxTmp;
  float minTmp;
	bool thunder;
	float windSpeed;
	float cloudCover;
};

static const Weather defaultWeather = {
  .summary = "No weather yet",
  .type = 0,
  .precipChance = 0.0f,
  .precipIntensity = 0.0f,
  .precipType = Rain,
  .maxTmp = 20.0f,
  .minTmp = 20.0f,
  .thunder = false,
  .windSpeed = 0.0f,
  .cloudCover = 0.0f
};


#endif
