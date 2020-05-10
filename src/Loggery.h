#pragma once

#define LOGMEM Serial.printf("FreeMem %d -- line %d - %s - %s\n",ESP.getFreeHeap(),__LINE__,__func__,__FILE__)