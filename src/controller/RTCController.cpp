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

String RTCController::getCurrentDateTimeStr()
{
    return Helper::convertDateTimeToString(getCurrentDateTime());
}

void RTCController::setDateTime(const DateTime &dateTime)
{
    LOG("Set dateTime to %s", Helper::convertDateTimeToString(dateTime).c_str());
    setDate(dateTime.year(), dateTime.month(), dateTime.day());
    setTime(dateTime.hour(), dateTime.minute(), dateTime.second());
}

void RTCController::setTime(int hour, int minute, int second)
{
    if (Helper::isValidTime(hour, minute, second))
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
    if (Helper::isValidDate(year, month, day))
    {
        LOG("Set date to %d/%d/%d", year, month, day);
        mRTC->setYear(year - 2000);
        mRTC->setMonth(month);
        mRTC->setDate(day);
    }
    else
    {
        LOG("Invalid date: %d/%d/%d", year, month, day);
    }
}