#pragma once
#include <Esp.h>
#include <esp_log.h>

#define LOGMEM ESP_LOGI("MEM","High water %d --- FreeMem %d -- line %d - %s - %s",uxTaskGetStackHighWaterMark(NULL),ESP.getFreeHeap(),__LINE__,__func__,__FILE__)

