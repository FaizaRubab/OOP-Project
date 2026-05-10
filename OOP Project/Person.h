#pragma once

class Person 
{
    protected:
        int id;
        char name[100];
        char contact[20];
        char password[50];
    public:
        Person();
        Person(int, const char*, const char*, const char*);
        virtual ~Person();

        // Getters
        int getId()const;
        const char* getName() const;
        const char* getContact() const;

        // Pure virtual methods (must be overridden)
        virtual void displayMenu() = 0;
        virtual void saveToFile() = 0;

        bool checkPassword(const char*) const;
        static void strCopy(char*, const char*, int);
        static int strLen(const char*);
        static bool strEqual(const char*, const char*);

    protected:
        // Case-insensitive comparison helper
        static bool strEqualIgnoreCase(const char*, const char*);
};