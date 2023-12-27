#ifndef _RTC_CONTROLLER_H_
#define _RTC_CONTROLLER_H_

#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include "../common/Log.h"
#include "../common/Helper.h"

class RTCController
{
private:
    static DS3231 *mRTC;

public:
    static void init();

    static void getCurrentTime(int &hour, int &minute, int &second);
    static void getCurrentDate(int &year, int &month, int &day);
    static DateTime getCurrentDateTime();
    static String getCurrentDateTimeStr();

    static void setDateTime(const DateTime &dateTime);
    static void setTime(int hour, int minute, int second);
    static void setDate(int year, int month, int day);

private:
    static void checkDeviceIsActive();
};

#endif