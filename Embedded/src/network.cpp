#include "network.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
// MARK: NETWORK STUFF --------------------------------------

#define TAG "Network"

uint32_t lastConnectAttempt = 0;
bool wifiStopped = false;

bool reconnect(){
  // Restart wifi if we are power-saving
  if (wifiStopped){
    esp_wifi_start();
  }
  if (!WiFi.isConnected()){
    WiFi.reconnect();
  }

  return waitForWifi(2000);
}

bool waitForWifi(uint32_t milliseconds){
  
  uint64_t timeout = millis() + milliseconds;
  wl_status_t lastStatus = WL_NO_SHIELD;
  while (millis() < timeout){
    wl_status_t status = WiFi.status();
    if (status != lastStatus){
      lastStatus = status;
      ESP_LOGI(TAG,"Network status changed to: %s",wl_status_toString(status));

      if (status == WL_CONNECTED){
        return true;
      }
      
    }
    vTaskDelay(10);
  }
  ESP_LOGI(TAG,"Wifi timeout");
  return false;
}

void stopWifi(){
  ESP_LOGI(TAG, "Stopping wifi");
  esp_wifi_stop();
  wifiStopped = true;
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  ESP_LOGI(TAG,"SSID: %s",WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  ESP_LOGI(TAG,"IP Address: %s",ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  ESP_LOGI(TAG,"signal strength (RSSI): %l dBm",rssi);

}

const char* wl_status_toString(wl_status_t status){
  switch (status) {
    case WL_NO_SHIELD:
      return "WL_NO_SHIELD";
      break;
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
      break;
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
      break;
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
      break;
    case WL_CONNECTED:
      return "WL_CONNECTED";
      break;
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
      break;
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
      break;
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
      break;
  }
  return "UNKNOWN STATUS";
}