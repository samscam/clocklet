#pragma once
#include <Esp.h>
#include <esp_log.h>

#define LOGMEM ESP_LOGI("MEM","FreeMem %d -- line %d - %s - %s",ESP.getFreeHeap(),__LINE__,__func__,__FILE__)

