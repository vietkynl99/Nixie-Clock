#ifndef _RTC_CONTROLLER_H_
#define _RTC_CONTROLLER_H_

#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include "../common/Log.h"

class RTCController
{
private:
    static DS3231 *mRTC;

public:
    static void init();

    static void getCurrentTime(int &hour, int &minute, int &second);
    static void getCurrentDate(int &year, int &month, int &day);
    static DateTime getCurrentDateTime();
    static String getString(const DateTime &dateTime);

    static void setDateTime(const DateTime &dateTime);
    static void setTime(int hour, int minute, int second);
    static void setDate(int year, int month, int day);

    static bool isValid(const DateTime &dateTime);

private:
    static bool isValidTime(int hour, int minute, int second);
    static bool isValidDate(int year, int month, int day);
};

#endif