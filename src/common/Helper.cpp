#include "../../include/common/Helper.h"

int Helper::mTestValue = 0;

String Helper::convertDateTimeToString(const DateTime &dateTime)
{
    return String(dateTime.hour()) + ":" + String(dateTime.minute()) + ":" + String(dateTime.second()) + " " +
           String(dateTime.year()) + "/" + String(dateTime.month()) + "/" + String(dateTime.day());
}


bool Helper::isValidDateTime(const DateTime &dateTime)
{
    return isValidTime(dateTime.hour(), dateTime.minute(), dateTime.second()) &&
           isValidDate(dateTime.year(), dateTime.month(), dateTime.day());
}

bool Helper::isValidTime(int hour, int minute, int second)
{
    return hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59 &&
           second >= 0 && second <= 59;
}

bool Helper::isValidDate(int year, int month, int day)
{
    if (year < 2000 || year > 2099 || month < 1 || month > 12 || day < 1 || day > 31)
    {
        return false;
    }

    int maxDays = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        maxDays = 30;
    }
    else if (month == 2)
    {
        maxDays = year % 4 == 0 ? 29 : 28;
    }

    return (day <= maxDays);
}