#ifdef _MSC_VER
#  pragma warning(disable : 4996)
#endif

#include "Validator.h"
#include "Appointment.h"
#include <ctime>

bool Validator::allDigits(const char* str) 
{
    if (!str || str[0] == '\0')
        return false;
    for (int i = 0; str[i]; ++i)
        if (str[i] < '0' || str[i] > '9')
            return false;
    return true;
}
bool Validator::isValidContact(const char* contact) 
{
    int len = 0;
    while (contact[len]) 
        ++len;
    if (len != 11) 
        return false;
    return allDigits(contact);
}
bool Validator::isValidPassword(const char* password) 
{
    int len = 0;
    while (password[len]) 
        ++len;
    return len >= 6;
}
bool Validator::isPositiveFloat(const char* str) 
{
    if (!str || str[0] == '\0')
        return false;
    int dotCount = 0;
    bool hasDigit = false;
    for (int i = 0; str[i]; ++i) 
    {
        if (str[i] == '.') 
        {
            if (++dotCount > 1) 
                return false; 
        }
        else if (str[i] >= '0' && str[i] <= '9')
            hasDigit = true;
        else 
            return false;
    }
    if (!hasDigit) 
        return false;
    return parseFloat(str) > 0.f;
}
bool Validator::isPositiveInt(const char* str)
{
    if (!allDigits(str))
        return false;
    return parseInt(str) > 0;
}
bool Validator::isValidMenuChoice(const char* str, int lo, int hi, int& outVal) 
{
    if (!allDigits(str))
        return false;
    outVal = parseInt(str);
    return outVal >= lo && outVal <= hi;
}
float Validator::parseFloat(const char* str) 
{
    float result = 0.f;
    bool dec = false; 
    float scale = 0.1f;
    for (int i = 0; str[i]; ++i) 
    {
        if (str[i] == '.') 
        {
            dec = true;
            continue; 
        }
        float d = (float)(str[i] - '0');
        if (!dec)
            result = result * 10.f + d;
        else 
        {
            result += d * scale; 
            scale *= 0.1f;
        }
    }
    return result;
}
int Validator::parseInt(const char* str) 
{
    int v = 0;
    for (int i = 0; str[i]; ++i)
        v = v * 10 + (str[i] - '0');
    return v;
}
bool Validator::isValidTimeSlot(const char* slot) 
{
    return Appointment::isValidSlot(slot);
}
bool Validator::isValidDate(const char* date) 
{
    int len = 0;
    while (date[len]) 
        ++len;
    if (len != 10) 
        return false;
    if (date[2] != '-' || date[5] != '-') 
        return false;
    int positions[] = { 0,1,3,4,6,7,8,9 };
    for (int i = 0; i < 8; ++i)
        if (date[positions[i]] < '0' || date[positions[i]] > '9')
            return false;

    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int mon = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');

    if (day < 1 || day > 31) 
        return false;
    if (mon < 1 || mon > 12)
        return false;
    time_t now = time(nullptr);
    struct tm* lt = localtime(&now);
    int curYear = lt->tm_year + 1900;
    if (year < curYear) return false;
    return true;
}