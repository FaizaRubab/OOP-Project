#pragma once
#include "Person.h"  // for strCopy

class HospitalException {
protected:
    char message[200];

public:
    HospitalException() { message[0] = '\0'; }
    explicit HospitalException(const char* msg) {
        Person::strCopy(message, msg, 200);
    }
    virtual ~HospitalException() {}
    virtual const char* what() const { return message; }
};
class FileNotFoundException : public HospitalException 
{
public:
    explicit FileNotFoundException(const char* filename)
    {
        // Manual message composition
        char buf[200];
        int pos = 0;
        const char* prefix = "File not found: ";
        for (int i = 0;
            prefix[i] && pos < 199; ++i) buf[pos++] = prefix[i];
        for (int i = 0;
            filename[i] && pos < 199; ++i) buf[pos++] = filename[i];
        buf[pos] = '\0';
        Person::strCopy(message, buf, 200);
    }
};

class InsufficientFundsException : public HospitalException 
{
public:
    InsufficientFundsException()
        : HospitalException("Insufficient funds. Please top up your balance.") {}
};

class InvalidInputException : public HospitalException 
{
public:
    explicit InvalidInputException(const char* detail)
        : HospitalException(detail) {}
    InvalidInputException()
        : HospitalException("Invalid input. Please try again.") {}
};

class SlotUnavailableException : public HospitalException 
{
public:
    SlotUnavailableException(): HospitalException("This time slot is already booked. Please choose another.") { }
};