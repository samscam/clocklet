#include "Provisioning.h"

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include <nvs.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include "BlueStuff.h"

BlueStuff *blueStuff;

TaskHandle_t blueStuffTask;


bool provisioningIsActive = false;

void wifi_init_sta()
{
    /* Start wifi in station mode with credentials set during provisioning */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start() );
}

esp_err_t app_prov_is_provisioned(bool *provisioned)
{
    *provisioned = false;

#ifdef CONFIG_RESET_PROVISIONED
    nvs_flash_erase();
#endif

    if (nvs_flash_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS");
        return ESP_FAIL;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init wifi");
        return ESP_FAIL;
    }

    /* Get WiFi Station configuration */
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    if (strlen((const char*) wifi_cfg.sta.ssid)) {
        *provisioned = true;
        ESP_LOGI(TAG, "Found ssid %s",     (const char*) wifi_cfg.sta.ssid);
        ESP_LOGI(TAG, "Found password %s", (const char*) wifi_cfg.sta.password);
    }
    return ESP_OK;
}


bool isAlreadyProvisioned(){

    bool provisioned;
    
    if (app_prov_is_provisioned(&provisioned) != ESP_OK) {
        ESP_LOGE(TAG, "Error getting device provisioning state");
        return false;
    }
    return provisioned;
}

void doBackgroundThings(void * parameter) {


  blueStuff->startBlueStuff();
  
  while (true) {
    delay(5);
  }
}


void startProvisioning() {

    if (provisioningIsActive){
        return;
    }

    provisioningIsActive = true;

    BaseType_t core = 0;

    ESP_LOGE(TAG, "Starting provisioning");
    blueStuff = new BlueStuff();
    // blueStuff->startBlueStuff();
    
      
  xTaskCreatePinnedToCore(
    doBackgroundThings,
    "BlueStuffTask",
    5000,
    NULL,
    1,
    &blueStuffTask,
    core);

}

void stopProvisioning(){
    if (!provisioningIsActive){
        return;
    }

    provisioningIsActive = false;
    blueStuff->stopBlueStuff();
    delete(blueStuff);
    vTaskDelete(blueStuffTask);
}

bool isProvisioningActive() {
    return provisioningIsActive;
}