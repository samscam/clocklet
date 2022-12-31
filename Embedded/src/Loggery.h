#pragma once
#include <Esp.h>
#include <esp_log.h>

#define LOGMEM ESP_LOGI("MEM","High water %d --- FreeMem %d -- maxAllocHeap %d",uxTaskGetStackHighWaterMark(NULL),ESP.getFreeHeap(),ESP.getMaxAllocHeap())

