#include "TimeUtils.h"

double decimalise(DateTime time){
    double val;
    double secondsSinceStartOfDay = time.hour()*60*60 + time.minute()*60 + time.second();
    val = (secondsSinceStartOfDay / 86400.0)*10;
    return val;
}