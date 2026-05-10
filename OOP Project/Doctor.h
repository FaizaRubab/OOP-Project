#pragma once
#include "Person.h"
#include <ostream>

class Doctor : public Person
{
private:
    char  specialization[100];
    float fee;
public:
    Doctor();
    Doctor(int, const char*, const char*,const char*, const char*, float);
    ~Doctor() override;

    // Getters
    const char* getSpecialization() const;
    float getFee()const;
    int getId() const;

    // Case-insensitive
    bool matchesSpecialization(const char*) const;

    //Operator overloaded
    bool operator==(const Doctor&) const;
    friend std::ostream& operator<<(std::ostream&, const Doctor&);

    // Pure virtuals
    void displayMenu() override;
    void saveToFile() override;

    // File I/O helpers
    void   toFileLine(char*, int) const;
    static Doctor fromFileLine(const char*);
};