#pragma once
#include "Person.h"
#include <ostream>

class Patient : public Person 
{
private:
    int age;
    char* gender;   // Male OR Female
    float balance;
public:
    Patient();
    Patient(int, const char*, int, char,const char*, const char*, float);
    Patient(const Patient&);
    Patient& operator=(const Patient&);
    ~Patient() override;

    // Getters
    int getAge()const;
    char getGender()const;
    float getBalance()const;

    // Setters
    void setBalance(float);

    //Operator overloads
    Patient& operator+=(float);         
    Patient& operator-=(float);         
    bool operator==(const Patient&) const;  
    friend std::ostream& operator<<(std::ostream&, const Patient&);

    // Pure virtuals from Person
    void displayMenu() override;
    void saveToFile() override;

    // Serialise to comma-separated line
    void toFileLine(char*, int) const;

    // Deserialise from comma-separated line
    static Patient fromFileLine(const char*);
};
