#include "UpdateScheduler.h"
#include "Loggery.h"
// Start the update sheduler task

#define TAG "UPDATES"

UpdateScheduler::UpdateScheduler():Task("UpdateScheduler", 5000,  5){
    this->setCore(1); // Run it on core ONE
}

void UpdateScheduler::run(void *data) {
    
    // Get the time for the start of the cycle
    TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(true){

        for (auto it = _jobs.begin(); it != _jobs.end(); it++) {
            (*it)->update(esp_timer_get_time());
            delay(1);
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
    
}

void UpdateScheduler::addJob(UpdateJob *job, UpdateFrequency freq){
    job->frequency = freq; //not sure this is working :/
    _jobs.push_back(job);
}

UpdateJob::UpdateJob(){

}

void UpdateJob::update(int64_t startTime){
    
    // Too early, quickly return
    if (nextUpdateTime > startTime) {
        return;
    }

    // Must be time, let's try it
    LOGMEM;
    if (this->performUpdate()){
        ESP_LOGI(TAG, "Job frequency: %lld - in micros %lld",frequency,timeS_TO_MicroS(frequency));
        ESP_LOGI(TAG, "Start: %lld",startTime);
        nextUpdateTime = esp_timer_get_time() + timeS_TO_MicroS(frequency);
        ESP_LOGI(TAG, "Nextupdate: %lld",nextUpdateTime);
        retryCount = 0;
        return;
    }
    
    // The update failed - try again - backoff at 10 seconds per retry
    retryCount++;
    nextUpdateTime = startTime + timeS_TO_MicroS(retryCount * 10);
    ESP_LOGI(TAG, "Start: %lld",startTime);
    ESP_LOGI(TAG, "Retry %d backing off to %d",retryCount, retryCount*10);
    ESP_LOGI(TAG, "Nextupdate: %lld",nextUpdateTime);
}

void UpdateJob::setNeedsUpdate(){
    // Reset the next update time to now
    nextUpdateTime = esp_timer_get_time();
    retryCount = 0;
}
