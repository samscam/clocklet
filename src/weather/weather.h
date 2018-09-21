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


#endif
