
#pragma once

#include "esp_heap_caps.h"
#include <ArduinoJson.h>

struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }
  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }
};

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;