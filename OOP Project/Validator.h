#pragma once

class Validator 
{
public:
    static bool isValidDate(const char* date);
    static bool isValidTimeSlot(const char* slot);
    static bool isValidContact(const char* contact);
    static bool isValidPassword(const char* password);
    static bool isPositiveFloat(const char* str);
    static bool isPositiveInt(const char* str);
    static bool isValidMenuChoice(const char* str, int lo, int hi, int& outVal);
    static float parseFloat(const char* str);
    static int   parseInt(const char* str);
    static bool allDigits(const char* str);
private:
    Validator() = delete;
};