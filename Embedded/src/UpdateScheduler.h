#pragma once

#include "Utilities/Task.h"
#include <vector>
#include <memory>

#define timeS_TO_MicroS(xTimeInSeconds) (((int64_t) xTimeInSeconds) * ((int64_t) 1000000))

enum UpdateFrequency: int64_t{
    tensecs = 10,
    minutely = 60,
    hourly = 60*60,
    daily = 60*60*24,
    weekly = 60*60*24*7,
    startup = INT_MAX
};


class UpdateJob {
    public:
        UpdateJob();
        virtual ~UpdateJob() {};
        virtual bool performUpdate() = 0;
        
        void update(int64_t startTime);
        void setNeedsUpdate();
        const char* name;
        UpdateFrequency frequency;
    private:
        
        int retryCount = 0;
        int64_t nextUpdateTime = 0;

};

class UpdateScheduler: public Task {
public:
    UpdateScheduler();
    void run(void *data);

    void addJob(UpdateJob *job, const char* name, UpdateFrequency freq);

private:
    std::vector<UpdateJob*> _jobs;
};