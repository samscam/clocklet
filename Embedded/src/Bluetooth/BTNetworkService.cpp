#include "BTNetworkService.h"


#define MAX_NETS 128


#include <esp_wifi.h>
#include <esp_log.h>
#include "Loggery.h"

#define TAG "BTNetworkService"

#define CH_CURRENTNETWORK_UUID "BEB5483E-36E1-4688-B7F5-EA07361B26A8"
#define CH_AVAILABLENETWORKS_UUID "AF2B36C7-0E65-457F-A8AB-B996B656CF32"
#define CH_JOINNETWORK_UUID "DFBDE057-782C-49F8-A054-46D19B404D9F"


NetworkScanTask::NetworkScanTask(BLECharacteristic *availableNetworks) : Task("NetworkScan", 10000,  5){
    this->setCore(0);
    ch_availableNetworks = availableNetworks;
}

void NetworkScanTask::run(void *data){

    for (;;){
        _performWiFiScan();
        delay(5000);
    }
}

void NetworkScanTask::_performWiFiScan(){
    LOGMEM;
    
    bool runningScan = (WiFi.scanNetworks(true,false,true) == WIFI_SCAN_RUNNING);
    int networkCount = 0;

    while (runningScan){
        int16_t scanComplete = WiFi.scanComplete();

        if (scanComplete == WIFI_SCAN_FAILED) {
            ESP_LOGE(TAG,"SCAN FAILED");
            return;
        }

        if (scanComplete >= 0){
            networkCount = scanComplete;
            runningScan = false;
        }
        delay(100);
        
    }


    for (int i=0;(i<networkCount && i<MAX_NETS);i++){
        StaticJsonDocument<512> doc;
        NetworkInfo netInfo = {0};
        netInfo.index = i;

        WiFi.getNetworkInfo(netInfo.index,netInfo.ssid,netInfo.enctype,netInfo.rssi,netInfo.bssid,netInfo.channel);

        ESP_LOGI(TAG,"Found %s (ch %d rssi %d)\n",netInfo.ssid.c_str(),netInfo.channel,netInfo.rssi);

        _encodeNetInfo(doc,netInfo);
        char buffer[512];
        size_t len = serializeJson(doc,buffer,512);

        ch_availableNetworks->setValue((uint8_t*)buffer,len);
        ch_availableNetworks->notify(true);
        delay(20);
    }

    LOGMEM;
}

void NetworkScanTask::_encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo){
    // JsonObject obj = doc.createNestedObject();
    doc["ssid"]=netInfo.ssid;
    doc["enctype"]=netInfo.enctype;
    doc["channel"]=netInfo.channel;
    doc["rssi"]=netInfo.rssi;
    doc["bssid"]=_mac2String(netInfo.bssid);
}

// This bit is horrible - really something else should be doing the job of catching wifi status updates and distributing them around via a queue
BTNetworkService *btNetworkServiceInstance;
void wifiEventCb(WiFiEvent_t event)
{
    // callback needs to definitely be on the right processor too!!!
    if (btNetworkServiceInstance){
        btNetworkServiceInstance->wifiEvent(event);
    }
}

// Initialise network related service and characteristics
BTNetworkService::BTNetworkService(BLEServer *pServer, QueueHandle_t networkChangedQueue, QueueHandle_t networkStatusQueue){
    _networkChangedQueue = networkChangedQueue;
    _networkStatusQueue = networkStatusQueue;

    btNetworkServiceInstance = this;


    ESP_LOGI(TAG, "Starting network service %s",SV_NETWORK_UUID);
    sv_network = pServer->createService(SV_NETWORK_UUID);

    ch_currentNetwork = sv_network->createCharacteristic(
                                            CH_CURRENTNETWORK_UUID,
                                            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::NOTIFY
                                        );
    _updateCurrentNetwork();
    ch_currentNetwork->setCallbacks(this);

    ch_availableNetworks = sv_network->createCharacteristic(
                                            CH_AVAILABLENETWORKS_UUID,
                                            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
                                        );
    ch_availableNetworks->setCallbacks(this);

    ch_joinNetwork = sv_network->createCharacteristic(
                                            CH_JOINNETWORK_UUID,
                                            NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC
                                        );
    ch_joinNetwork->setCallbacks(this);

    sv_network->start();

    _networkScanTask = new NetworkScanTask(ch_availableNetworks);

}


void BTNetworkService::onWrite(BLECharacteristic* pCharacteristic) {

    if (pCharacteristic != ch_joinNetwork){
        return;
    }

    LOGMEM;
    std::string msg = pCharacteristic->getValue();
    ESP_LOGI(TAG,"BLE received: %s\n", msg.c_str());
    // ESP_LOGI(LOG_TAG, "BLE received: %s", msg.c_str());

    StaticJsonDocument<512> doc;
    deserializeJson(doc,msg);
    
    // Fish out and validate the SSID
    const char * ssid = doc["ssid"];
    if (strlen(ssid) == 0 || strlen(ssid) > 32) {
        ESP_LOGI(TAG,"Invalid ssid length");
        return;
    }
    
    uint8_t  enctype = doc["enctype"];
    // Note: if the enctype is 255 we are trying to join something with unknown security
    
    // If it's an open network...
    if (enctype == 0){
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid);
        return;
    }


    // Validate the PSK
    // FUDGE - this also depends on the security type
    // For WPA2-PSK we are on an 8-63 char ascii phrase or 64 hex digits
    const char * psk = doc["psk"];
    if (strlen(psk) < 8 || strlen(psk) > 64) {
        ESP_LOGI(TAG, "BLE received: %s", msg.c_str());
        return;
    }

    // Attempt to connect to the new network !!!!
    WiFi.disconnect();

    delay(1000);
    WiFi.begin(ssid,psk);

}

void BTNetworkService::onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue){
    if (pCharacteristic == ch_availableNetworks){
        if (subValue > 0){
            ESP_LOGI(TAG,"Subscibing to availableNetworks");
            _networkScanTask->start();
        }
        if (subValue == 0){
            ESP_LOGI(TAG,"Unsubscribing to availableNetworks");
            _networkScanTask->stop();
        }
    }

    if (pCharacteristic == ch_currentNetwork){
        if (subValue > 0 && !_wifiEvent){
            ESP_LOGI(TAG,"Subscibing to wifiEvent");
            _wifiEvent = WiFi.onEvent(wifiEventCb);
            _updateCurrentNetwork();
        }
        if (subValue == 0 && _wifiEvent){
            ESP_LOGI(TAG,"Unsubscribing to wifiEvent");
            WiFi.removeEvent(_wifiEvent);
            _wifiEvent = 0;
        }
    }
}

void BTNetworkService::_updateCurrentNetwork(){
    
    StaticJsonDocument<512> doc;

    bool configured = false;

    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) == ESP_OK) {
        const char *ssid = (const char *) wifi_cfg.sta.ssid;
        if (strlen(ssid)) {
            configured = true;
            doc["ssid"]=ssid;
        }
    }
    
    doc["configured"] = configured;
    doc["status"]=(int)WiFi.status();
    doc["connected"]=WiFi.isConnected();
    doc["channel"]=WiFi.channel();
    doc["ip"]=WiFi.localIP().toString();
    doc["rssi"]=WiFi.RSSI();

    String outputStr = "";
    serializeJson(doc,outputStr);
    uint len = outputStr.length()+1;
    char json[len];
    outputStr.toCharArray(json,len);

    ch_currentNetwork->setValue(json);
    ch_currentNetwork->notify(true);
}



void BTNetworkService::wifiEvent(WiFiEvent_t event){
    
    ESP_LOGD(TAG,"[WiFi-event] event: %d\n", event);
    _updateCurrentNetwork();

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            ESP_LOGD(TAG,"WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            ESP_LOGD(TAG,"Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            ESP_LOGD(TAG,"WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            ESP_LOGD(TAG,"WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGD(TAG,"Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGD(TAG,"Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            ESP_LOGD(TAG,"Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGD(TAG,"Obtained IP address: %s",WiFi.localIP().toString().c_str());
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            ESP_LOGD(TAG,"Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            ESP_LOGD(TAG,"WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            ESP_LOGD(TAG,"WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            ESP_LOGD(TAG,"WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            ESP_LOGD(TAG,"WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            ESP_LOGD(TAG,"AP WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            ESP_LOGD(TAG,"AP WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGD(TAG,"AP Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGD(TAG,"AP Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            ESP_LOGD(TAG,"AP Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            ESP_LOGD(TAG,"AP Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            ESP_LOGD(TAG,"IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            ESP_LOGD(TAG,"Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            ESP_LOGD(TAG,"Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            ESP_LOGD(TAG,"Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            ESP_LOGD(TAG,"Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            ESP_LOGD(TAG,"Ethernet Obtained IP address");
            break;
        default: break;
    }
        
}





String _mac2String(uint8_t * bytes)
{
  String s;
  int len = sizeof(bytes);
  for (byte i = 0; i < len; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", bytes[i]);
    s += buf;
    if (i < len-1) s += ':';
  }
  return s;
}
