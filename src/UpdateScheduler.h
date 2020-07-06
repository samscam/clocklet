#pragma once

#include "Utilities/Task.h"
#include <vector>
#include <memory>

#define timeS_TO_MicroS(xTimeInSeconds) (xTimeInSeconds * 1000000)

enum UpdateFrequency{
    minutely = 60,
    hourly = 60*60,
    daily = 60*60*24,
    weekly = 60*60*24*7,
    startup = INT_MAX
};


class UpdateJob {
    public:
        UpdateJob() {};
        virtual ~UpdateJob() {};

        virtual bool performUpdate();
        
        void update(int64_t startTime);
        void setNeedsUpdate();

        UpdateFrequency frequency;
    private:
        
        int retryCount = 0;
        time_t nextUpdateTime = 0;

};

class UpdateScheduler: public Task {
public:
    UpdateScheduler();
    void run(void *data);

    void addJob(UpdateJob *job, UpdateFrequency freq);

private:
    std::vector<UpdateJob*> _jobs;
};