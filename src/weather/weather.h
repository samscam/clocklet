#ifndef PRISCILLA_WEATHER
#define PRISCILLA_WEATHER


enum PrecipType {
	Rain, Snow, Sleet, Hail
};

struct weather {
  const char* summary;
  const char* type;
  float precipChance;
  PrecipType precipType;
  float maxTmp;
  float minTmp;
};



#include "darksky.h"

#endif
