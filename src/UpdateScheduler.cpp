#include "UpdateScheduler.h"

// Start the update sheduler task

UpdateScheduler::UpdateScheduler():Task("UpdateScheduler", 10000,  5){
    this->setCore(0); // Run it on core zero
}

void UpdateScheduler::run(void *data) {
    
    // Get the time for the start of the cycle
    TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(true){

        for (auto it = _jobs.begin(); it != _jobs.end(); it++) {
            (*it)->update(esp_timer_get_time());
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
    
}

void UpdateScheduler::addJob(UpdateJob *job, UpdateFrequency freq){
    job->frequency = freq;
    _jobs.push_back(job);
}

void UpdateJob::update(int64_t startTime){
    
    // Too early, quickly return
    if (nextUpdateTime > startTime) {
        return;
    }

    // Must be time, let's try it
    if (this->performUpdate()){
        nextUpdateTime = startTime + timeS_TO_MicroS(frequency);
        retryCount = 0;
        return;
    }
    
    // The update failed - try again - backoff at 10 seconds per retry
    retryCount++;
    nextUpdateTime = startTime + timeS_TO_MicroS(retryCount * 10);

}

void UpdateJob::setNeedsUpdate(){
    // Reset the next update time to now
    nextUpdateTime = esp_timer_get_time();
    retryCount = 0;
}

// void UpdateScheduler::weatherDidUpdate(){
//     display->setWeather(weatherClient->horizonWeather);
//     rainbows.setWeather(weatherClient->rainbowWeather);
// }

// void UpdateScheduler::firmware(){
//     if (reconnect()) {
//     FirmwareUpdates *firmwareUpdates = new FirmwareUpdates;
//     Preferences preferences = Preferences();
//     preferences.begin("clocklet", true);
//     bool staging = preferences.getBool("staging",false);
//     if (firmwareUpdates->checkForUpdates(staging)){
//       if (firmwareUpdates->updateAvailable){
//         display->displayMessage("Updating Firmware", rando);
//         display->setStatusMessage("wait");
//         if (!firmwareUpdates->startUpdate()){
//           display->displayMessage("Update failed... sorry",bad);
//         }
//       }
//     } else {
//       ESP_LOGI("CORE","Update check failed");
//     }
//     preferences.end();

//     delete firmwareUpdates;
//     Serial.println("Firmware update done");
//     LOGMEM;
//   }
// }

// void UpdateScheduler::timesync(){

//     // Hourly sync the system (ntp) time back to the ds3231
//     Serial.println("Syncing time: esp32 >> ds3231");
//     uint32_t u32 = rtc.now().unixtime();
//     while (rtc.now().unixtime() == u32){
//     delay(1);
//     }
//     DateTime timertc = rtc.now();
//     ds3231.adjust(timertc);

//     char ds3231_buf[64] = "DDD, DD MMM YYYY hh:mm:ss";
//     char esp32_buf[64] =  "DDD, DD MMM YYYY hh:mm:ss";
//     Serial.printf("Sync complete... time is:\n - ds3231: %s\n - esp32: %s\n",ds3231.now().toString(ds3231_buf),rtc.now().toString(esp32_buf));

// }