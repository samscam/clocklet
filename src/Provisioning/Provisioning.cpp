#include "Provisioning.h"

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include "app_prov.h"


void WiFiEvent(system_event_id_t event, system_event_info_t info){
    system_event_t systemEvent = {event,info};
    void *ctx = NULL;

    // Invoke the provisioning event handler
    app_prov_event_handler(ctx, &systemEvent);

    // Some leftover logging... mostly
    switch(event) {
    case SYSTEM_EVENT_AP_START:
        ESP_LOGE(TAG, "SoftAP started");
        break;
    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGE(TAG, "SoftAP stopped");
        break;
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGE(TAG, "got ip:%s",
                 ip4addr_ntoa(&info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGE(TAG, "station:" MACSTR " join, AID=%d",
                 MAC2STR(info.sta_connected.mac),
                 info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGE(TAG, "station:" MACSTR "leave, AID=%d",
                 MAC2STR(info.sta_disconnected.mac),
                 info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        break;
    default:
        break;
    }
}


void wifi_init_sta()
{
    /* Start wifi in station mode with credentials set during provisioning */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start() );
}

bool isAlreadyProvisioned(){
    bool provisioned;
    if (app_prov_is_provisioned(&provisioned) != ESP_OK) {
        ESP_LOGE(TAG, "Error getting device provisioning state");
        return false;
    }
    return provisioned;
}

void startProvisioning() {
    
    ESP_LOGE(TAG, "Starting provisioning");

    /* Security version */
    int security = 0;
    /* Proof of possession */
    const protocomm_security_pop_t *pop = NULL;

#ifdef CONFIG_USE_SEC_1
    security = 1;
#endif

    /* Having proof of possession is optional */
#ifdef CONFIG_USE_POP
    const static protocomm_security_pop_t app_pop = {
        .data = (uint8_t *) CONFIG_POP,
        .len = (sizeof(CONFIG_POP)-1)
    };
    pop = &app_pop;
#endif

    
    WiFi.onEvent(WiFiEvent);
    
    bool provisioned;
    if (app_prov_is_provisioned(&provisioned) != ESP_OK) {
        ESP_LOGE(TAG, "Error getting device provisioning state");
        return;
    }

    ESP_LOGE(TAG, "Starting WiFi SoftAP provisioning");

    const char *ssid = NULL;

#ifdef CONFIG_SOFTAP_SSID
    ssid = CONFIG_SOFTAP_SSID;
#else
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);

    char ssid_with_mac[33];
    snprintf(ssid_with_mac, sizeof(ssid_with_mac), "PROV_%02X%02X%02X",
                eth_mac[3], eth_mac[4], eth_mac[5]);

    ssid = ssid_with_mac;
#endif

    app_prov_start_softap_provisioning(ssid, CONFIG_SOFTAP_PASS,
                                        security, pop);

}