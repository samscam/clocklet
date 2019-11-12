
#include <Preferences.h>


#include "BlueStuff.h"
#include "esp_bt_device.h"

// #include "BLE2902.h"

#include <esp_log.h>
#include <Preferences.h>
#include "Loggery.h"

#include "../Location/LocationManager.h"

#define SV_NETWORK_UUID     "68D924A1-C1B2-497B-AC16-FD1D98EDB41F"
#define CH_CURRENTNETWORK_UUID "BEB5483E-36E1-4688-B7F5-EA07361B26A8"
#define CH_AVAILABLENETWORKS_UUID "AF2B36C7-0E65-457F-A8AB-B996B656CF32"
#define CH_JOINNETWORK_UUID "DFBDE057-782C-49F8-A054-46D19B404D9F"

#define SV_LOCATION_UUID "87888F3E-C1BF-4832-9823-F19C73328D30"
#define CH_CURRENTLOCATION_UUID "C8C7FF91-531A-4306-A68A-435374CB12A9"


BlueStuff *blueStuffInstance;

static char LOG_TAG[] = "BLUESTUFF";

class JoinNetworkCallback: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* pCharacteristic) {
        LOGMEM;
		std::string msg = pCharacteristic->getValue();
        Serial.printf("BLE received: %s\n", msg.c_str());
        // ESP_LOGI(LOG_TAG, "BLE received: %s", msg.c_str());

        StaticJsonDocument<512> doc;
        deserializeJson(doc,msg);
        
        // Fish out and validate the SSID
        const char * ssid = doc["ssid"];
        if (strlen(ssid) == 0 || strlen(ssid) > 32) {
            ESP_LOGI("Invalid ssid length");
            return;
        }

        // If we have no psk... let's see if it's an open network...
        if (doc["psk"].isNull()){
            WiFi.begin(ssid);
            return;
        }


        // Validate the PSK
        // FUDGE - this also depends on the security type
        // For WPA2-PSK we are on an 8-63 char ascii phrase or 64 hex digits
        const char * psk = doc["psk"];
        if (strlen(psk) < 8 || strlen(psk) > 64) {
            ESP_LOGI(LOG_TAG, "BLE received: %s", msg.c_str());
            return;
        }

        // Stop any in-progress scans...
        if (blueStuffInstance){
            blueStuffInstance->_shouldScan = false;
        }

        // Attempt to connect to the new network CRASHOLA!!!!
        WiFi.disconnect();
        delay(100);
        WiFi.begin(ssid,psk);

	}

};

class SetLocationCallback: public BLECharacteristicCallbacks {
    
    LocationManager *locationManager = new LocationManager();

	void onWrite(BLECharacteristic* pCharacteristic) {
        LOGMEM;
		std::string msg = pCharacteristic->getValue();
        Serial.printf("BLE received: %s\n", msg.c_str());

        StaticJsonDocument<512> doc;
        deserializeJson(doc,msg);
        
        const double lat = doc["lat"];
        if (isnan(lat)) {
            ESP_LOGI("Invalid latitude");
            return;
        }

        const double lng = doc["lng"];
        if (isnan(lng)) {
            ESP_LOGI("Invalid longitude");
            return;
        }
        Location newLocation = {};
        newLocation.lat = lat;
        newLocation.lng = lng;
        locationManager->setLocation(newLocation);

	}

    void onRead(BLECharacteristic* pCharacteristic) {
        LOGMEM;
		std::string msg = pCharacteristic->getValue();
		ESP_LOGI(LOG_TAG, "BLE received: %s, %i", msg.c_str(), msg.length());
		// esp_log_buffer_char(LOG_TAG, msg.c_str(), msg.length());
		// esp_log_buffer_hex(LOG_TAG, msg.c_str(), msg.length());
        Location location = locationManager->getLocation();

        StaticJsonDocument<80> locDoc;
        
        locDoc["lat"]=location.lat;
        locDoc["lng"]=location.lng;

        String outputStr = "";
        serializeJson(locDoc,outputStr);
        uint len = outputStr.length()+1;
        char json[len];
        outputStr.toCharArray(json,len);
        Serial.println(json);

        pCharacteristic->setValue(json);
	}
};


void BlueStuff::startBlueStuff(){
    LOGMEM;
    blueStuffInstance = this;

    Serial.println("Starting BLE work!");

    Preferences preferences = Preferences();
    preferences.begin("clocklet", true);
    uint32_t serial = preferences.getUInt("serial");
    if (isnan(serial)){
        serial = 0;
    }
    
    const char * shortName = "Clocklet";

    char * deviceName;
    asprintf(&deviceName,"%s #%d",shortName,serial);
    LOGMEM;
    BLEDevice::init(deviceName);
    LOGMEM;
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    LOGMEM;
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    LOGMEM;
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);
    LOGMEM;
    _startNetworkService();
    LOGMEM;
    _startLocationService();
    LOGMEM;
    // Start Advertising...
    BLEAdvertisementData adData;
    adData.setName(deviceName);
    // adData.setShortName(deviceName);
    // adData.setPartialServices(BLEUUID(SV_NETWORK_UUID));
    adData.setAppearance(256); // GENERIC CLOCK

    // This should really be in the correct format - with manufacturer id as the first bytes, etc etc.
    adData.setManufacturerData("1,bones");

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    
    // pAdvertising->setAdvertisementData(adData);
    pAdvertising->setScanResponseData(adData);

    pAdvertising->addServiceUUID(SV_NETWORK_UUID);
    
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);


    BLEDevice::startAdvertising();

    
    LOGMEM;
    while(true){
        delay(5000);
        if (_shouldScan){
            _startWifiScan();
        }
    }
}

void BlueStuff::stopBlueStuff(){
    
}

void BlueStuff::onConnect(BLEServer* server) {

    WiFi.onEvent(wifiEventCb);

    delay(2000);
    _shouldScan = true;
    _updateCurrentNetwork();
}

void BlueStuff::onDisconnect(BLEServer* server) {
    WiFi.removeEvent(wifiEventCb);
    _shouldScan = false;
}

void BlueStuff::_startNetworkService(){
    ESP_LOGI(LOG_TAG, "Starting network service %s",SV_NETWORK_UUID);
    sv_network = pServer->createService(SV_NETWORK_UUID);

    ch_currentNetwork = sv_network->createCharacteristic(
                                            CH_CURRENTNETWORK_UUID,
                                            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
                                        );
    ch_currentNetwork->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);

    ch_availableNetworks = sv_network->createCharacteristic(
                                            CH_AVAILABLENETWORKS_UUID,
                                            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
                                        );
    ch_availableNetworks->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);

    ch_joinNetwork = sv_network->createCharacteristic(
                                            CH_JOINNETWORK_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
    ch_joinNetwork->setAccessPermissions(ESP_GATT_PERM_WRITE_ENCRYPTED);
    ch_joinNetwork->setCallbacks(new JoinNetworkCallback());

    // BLE2902* p2902Descriptor = new BLE2902();
    // p2902Descriptor->setNotifications(true);
    // // ch_currentNetwork->addDescriptor(p2902Descriptor);
    // // ch_availableNetworks->addDescriptor(p2902Descriptor);
    // ch_joinNetwork->addDescriptor(p2902Descriptor);
    // /*
    //     * Authorized permission to read/write descriptor to protect notify/indicate requests
    //     */
    // p2902Descriptor->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);


    sv_network->start();


}

void BlueStuff::_startLocationService(){
    ESP_LOGI(LOG_TAG, "Starting location service %s",SV_LOCATION_UUID);
    sv_location= pServer->createService(SV_LOCATION_UUID);

    ch_currentLocation = sv_location->createCharacteristic(
                                            CH_CURRENTLOCATION_UUID,
                                            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
                                        );
    ch_currentLocation->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    ch_currentLocation->setCallbacks(new SetLocationCallback());


    sv_location->start();
}


void BlueStuff::_updateCurrentNetwork(){
    LOGMEM;
    StaticJsonDocument<200> doc;
    doc["status"]=(int)WiFi.status();
    doc["ssid"]=WiFi.SSID();
    doc["connected"]=WiFi.isConnected();
    doc["channel"]=WiFi.channel();
    doc["ip"]=WiFi.localIP().toString();
    doc["rssi"]=WiFi.RSSI();

    String outputStr = "";
    serializeJson(doc,outputStr);
    uint len = outputStr.length()+1;
    char json[len];
    outputStr.toCharArray(json,len);
    Serial.println(json);

    ch_currentNetwork->setValue(json);
    ch_currentNetwork->notify(true);
}

// Hasty wrapper... not clean at-all
void wifiEventCb(WiFiEvent_t event)
{
    // callback needs to definitely be on the right processor too!!!
    if (blueStuffInstance){
        blueStuffInstance->wifiEvent(event);
    }
}

void BlueStuff::wifiEvent(WiFiEvent_t event){
    
    // Serial.printf("[WiFi-event] event: %d\n", event);


    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            // Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            _updateCurrentNetwork();    
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("AP WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("AP WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("AP Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("AP Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("AP Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("AP Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            // _updateCurrentNetwork();    
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Ethernet Obtained IP address");
            break;
        default: break;
    }
        
}



void BlueStuff::_startWifiScan(){
    LOGMEM;
    bool runningScan = (WiFi.scanNetworks(true,true,false) == WIFI_SCAN_RUNNING);
    int networkCount = 0;

    while (runningScan){
        networkCount = WiFi.scanComplete();
        if (networkCount >= 0){
            runningScan = false;
        }
    }

    StaticJsonDocument<1024> doc;
    // DynamicJsonDocument doc(2048);
    // JsonArray array = doc.createNestedArray();

    for (int i=0;(i<networkCount && i<MAX_NETS);i++){
        NetworkInfo netInfo = {0};
        netInfo.index = i;

        WiFi.getNetworkInfo(netInfo.index,netInfo.ssid,netInfo.enctype,netInfo.rssi,netInfo.bssid,netInfo.channel);
        // networks.push_back(netInfo);
        // Serial.printf("Found %s (ch %d rssi %d)\n",netInfo.ssid,netInfo.channel,netInfo.rssi);

        _encodeNetInfo(doc,netInfo);
    }
    String outputStr = "";
    serializeJson(doc,outputStr);

    uint len = outputStr.length()+1;
    char availableJson[len];
    outputStr.toCharArray(availableJson,len);
    // Serial.println(availableJson);
    ch_availableNetworks->setValue(availableJson);
    ch_availableNetworks->notify(true);
    LOGMEM;
}

String mac2String(uint8_t * bytes)
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

void BlueStuff::_encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo){
    JsonObject obj = doc.createNestedObject();
    obj["ssid"]=netInfo.ssid;
    obj["enctype"]=netInfo.enctype;
    obj["channel"]=netInfo.channel;
    obj["rssi"]=netInfo.rssi;
    obj["bssid"]=mac2String(netInfo.bssid);
}



