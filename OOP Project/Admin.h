#pragma once
#include "Person.h"

class Admin : public Person 
{
public:
    Admin();
    Admin(int,const char*,const char*);
    ~Admin() override;

    void displayMenu() override;
    void saveToFile() override;

    static Admin fromFileLine(const char*);
};