#include "UpdateScheduler.h"
#include "Loggery.h"
// Start the update sheduler task

#define TAG "UPDATES"

#define MAX_RETRIES 5

#if defined(SPI_RAM)
UpdateScheduler::UpdateScheduler():Task("UpdateScheduler", 20480,  5){
    this->setCore(0);
}
#else
UpdateScheduler::UpdateScheduler():Task("UpdateScheduler", 8192,  5){
    this->setCore(0);
}
#endif

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

void UpdateScheduler::addJob(UpdateJob *job, const char* name, UpdateFrequency freq){
    job->frequency = freq;
    job->name = name;
    _jobs.push_back(job);
}

UpdateJob::UpdateJob(){

}

void UpdateJob::update(int64_t startTime){
    
    // Too early, quickly return
    if (nextUpdateTime > startTime) {
        return;
    }

    ESP_LOGI(TAG, "Running job: %s",name);
    // Must be time, let's try it
    LOGMEM;
    bool updateResult = this->performUpdate();
    LOGMEM;

    nextUpdateTime = esp_timer_get_time() + timeS_TO_MicroS(frequency);
    
    if (updateResult){
        retryCount = 0;
        ESP_LOGI(TAG, "Job %s ok - Nextupdate: %lld",name, nextUpdateTime);
        return;
    }

    // The update failed - try again - backoff at 10 seconds per retry
    retryCount++;

    if (retryCount >= MAX_RETRIES){
        //bail after max updates and run at normal frequency
        retryCount = 0;
        ESP_LOGI(TAG, "Job %s has exceeded max retries %d ... ",name,retryCount);
        ESP_LOGI(TAG, "Nextupdate: %lld",nextUpdateTime);
        return;
    } else {
        nextUpdateTime = startTime + timeS_TO_MicroS(retryCount * 10);
        ESP_LOGI(TAG, "Retrying %s : %d backing off to %d",name,retryCount, retryCount*10);
        ESP_LOGI(TAG, "Nextupdate: %lld",nextUpdateTime);
        return;
    }


}

void UpdateJob::setNeedsUpdate(){
    // Reset the next update time to now
    nextUpdateTime = esp_timer_get_time();
    retryCount = 0;
}
