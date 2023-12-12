#include "../../include/controller/RTCController.h"

DS3231 *RTCController::mRTC = new DS3231();

static constexpr const char *const TAG = "RTC";

void RTCController::init()
{
    Wire.begin();

    mRTC->setClockMode(false); // set to 24h
    // setClockMode(true); // set to 12h
}

void RTCController::getCurrentTime(int &hour, int &minute, int &second)
{
    bool format12h = false;
    bool formatPM = false;

    hour = mRTC->getHour(format12h, formatPM);
    minute = mRTC->getMinute();
    second = mRTC->getSecond();
}

void RTCController::getCurrentDate(int &year, int &month, int &day)
{
    bool formatCentury = false;

    year = mRTC->getYear() + 2000;
    month = mRTC->getMonth(formatCentury);
    day = mRTC->getDate();
}

DateTime RTCController::getCurrentDateTime()
{
    int hour, minute, second, year, month, day;
    getCurrentTime(hour, minute, second);
    getCurrentDate(year, month, day);
    return DateTime(year, month, day, hour, minute, second);
}

String RTCController::getString(const DateTime &dateTime)
{
    return String(dateTime.hour()) + ":" + String(dateTime.minute()) + ":" + String(dateTime.second()) + " " +
           String(dateTime.year()) + "/" + String(dateTime.month()) + "/" + String(dateTime.day());
}

void RTCController::setDateTime(const DateTime &dateTime)
{
    setDate(dateTime.year(), dateTime.month(), dateTime.day());
    setTime(dateTime.hour(), dateTime.minute(), dateTime.second());
}

void RTCController::setTime(int hour, int minute, int second)
{
    if (isValidTime(hour, minute, second))
    {
        LOG("Set time to %d:%d:%d", hour, minute, second);
        mRTC->setHour(hour);
        mRTC->setMinute(minute);
        mRTC->setSecond(second);
    }
    else
    {
        LOG("Invalid time: %d:%d:%d", hour, minute, second);
    }
}

void RTCController::setDate(int year, int month, int day)
{
    if (isValidDate(year, month, day))
    {
        LOG("Set time to %d/%d/%d", year, month, day);
        mRTC->setYear(year - 2000);
        mRTC->setMonth(month);
        mRTC->setDate(day);
    }
    else
    {
        LOG("Invalid date: %d/%d/%d", year, month, day);
    }
}

bool RTCController::isValid(const DateTime &dateTime)
{
    return isValidTime(dateTime.hour(), dateTime.minute(), dateTime.second()) &&
           isValidDate(dateTime.year(), dateTime.month(), dateTime.day());
}

bool RTCController::isValidTime(int hour, int minute, int second)
{
    return hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59 &&
           second >= 0 && second <= 59;
}

bool RTCController::isValidDate(int year, int month, int day)
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