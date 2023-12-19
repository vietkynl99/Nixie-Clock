#ifndef _HELPER_H_
#define _HELPER_H_

#include <Arduino.h>
#include <DS3231.h>

class Helper
{
public:
    static int mTestValue;

public:
    static String convertDateTimeToString(const DateTime &dateTime);
    static bool isValidDateTime(const DateTime &dateTime);
    static bool isValidTime(int hour, int minute, int second);
    static bool isValidDate(int year, int month, int day);
};

#endif