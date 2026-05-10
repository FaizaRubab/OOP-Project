
#include "Person.h"
#include <cctype>   // tolower

Person::Person() : id(0) 
{
    name[0] = contact[0] = password[0] = '\0';
}

Person::Person(int id, const char* name, const char* contact, const char* password):id(id) 
{
    strCopy(this->name, name, 100);
    strCopy(this->contact, contact, 20);
    strCopy(this->password, password, 50);
}

Person::~Person() {}

int Person::getId()const 
{
    return id; 
}
const char* Person::getName()const 
{ 
    return name;
}
const char* Person::getContact() const
{
    return contact; 
}
bool Person::checkPassword(const char* input) const 
{
    return strEqual(password, input);
}
bool Person::strEqual(const char* a, const char* b) 
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') 
    {
        if (a[i] != b[i])
        {
            return false;
        }
        ++i;
    }
    return a[i] == '\0' && b[i] == '\0';
}
void Person::strCopy(char* dst, const char* src, int maxLen) 
{
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0')
    {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = '\0';
}
int Person::strLen(const char* s) 
{
    int n = 0;
    while (s[n] != '\0')
    {
        ++n;
    }
    return n;
}
bool Person::strEqualIgnoreCase(const char* a, const char* b) 
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') 
    {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i]))
        {
            return false;
        }
        ++i;
    }
    return a[i] == '\0' && b[i] == '\0';
}