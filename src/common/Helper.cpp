#include "../../include/common/Helper.h"

int Helper::mTestValue = 0;

static constexpr const char *const TAG = "HELPER";

String Helper::convertDateToString(const DateTime &dateTime)
{
    String result = String(dateTime.year()) + "/";
    result += dateTime.month() > 9 ? String(dateTime.month()) : "0" + String(dateTime.month());
    result += "/";
    result += dateTime.day() > 9 ? String(dateTime.day()) : "0" + String(dateTime.day());
    return result;
}

String Helper::convertTimeToString(const DateTime &dateTime)
{
    String result = "";
    result += dateTime.hour() > 9 ? String(dateTime.hour()) : "0" + String(dateTime.hour());
    result += ":";
    result += dateTime.minute() > 9 ? String(dateTime.minute()) : "0" + String(dateTime.minute());
    result += ":";
    result += dateTime.second() > 9 ? String(dateTime.second()) : "0" + String(dateTime.second());
    return result;
}

String Helper::convertDateTimeToString(const DateTime &dateTime)
{
    return convertTimeToString(dateTime) + " " + convertDateToString(dateTime);
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

void Helper::showFreeMemory()
{
    static size_t totalHeap = ESP.getHeapSize();
    size_t freeHeap = ESP.getFreeHeap();

    LOGF("Free Heap: %d bytes (%d%% free)", freeHeap, (freeHeap * 100) / (totalHeap + 1));
}

void Helper::trim(String &str)
{
	while (str.length() > 0 && (str.indexOf(' ') == 0 || str.indexOf('\r') == 0 || str.indexOf('\n') == 0))
	{
		str = str.substring(1);
	}
	while (str.length() > 0 && (str.lastIndexOf(' ') == str.length() - 1 || str.lastIndexOf('\r') == str.length() - 1 || str.lastIndexOf('\n') == str.length() - 1))
	{
		str = str.substring(0, str.length() - 1);
	}
}