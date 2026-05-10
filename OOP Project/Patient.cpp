#include "Patient.h"
#include <ostream>

Patient::Patient() : Person(), age(0), balance(0.0f) 
{
  gender[0] = '\0';
}

Patient::Patient(int id, const char* name, int age, char gender,const char* contact, const char* password, float balance) : Person(id, name, contact, password), age(age), balance(balance) 
{
    this->gender[0] = gender;
    this->gender[1] = '\0';
}

Patient::~Patient() {}

int Patient::getAge()const 
{
    return age; 
}
char Patient::getGender()const 
{
    return gender[0]; 
}
float Patient::getBalance()const 
{
    return balance; 
}
void Patient::setBalance(float b) 
{
    balance = b; 
}
//Operator overloads 
Patient& Patient::operator+=(float amount) 
{
    balance += amount;
    return *this;
}
Patient& Patient::operator-=(float amount) 
{
    balance -= amount;
    return *this;
}
bool Patient::operator==(const Patient& other) const
{
    return id == other.id;
}
std::ostream& operator<<(std::ostream& out, const Patient& p) 
{
    out << "Patient ID : " << p.id << "\n"
        << "Name       : " << p.name << "\n"
        << "Age        : " << p.age << "\n"
        << "Gender     : " << p.gender << "\n"
        << "Contact    : " << p.contact << "\n"
        << "Balance    : PKR " << p.balance;
    return out;
}
void Patient::toFileLine(char* buf, int maxLen) const 
{
    // Format: patient_id,name,age,gender,contact,password,balance
    int pos = 0;
    auto writeInt = [&](int v) 
        {
        char tmp[20]; 
        int ti = 0;
        if (v < 0) 
        {
            buf[pos++] = '-';
            v = -v; 
        }
        if (v == 0) 
        {
            tmp[ti++] = '0'; 
        }
        while (v > 0) 
        {
            tmp[ti++] = '0' + (v % 10); 
            v /= 10; 
        }
        for (int j = ti - 1; j >= 0 && pos < maxLen - 1; --j)
        {
            buf[pos++] = tmp[j];
        }
        };
    auto writeStr = [&](const char* s) 
        {
            for (int i = 0; s[i] != '\0' && pos < maxLen - 1; ++i)
            {
                buf[pos++] = s[i];
            }
        };
    auto writeFloat = [&](float f) 
        {
        int whole = (int)f;
        int frac = (int)((f - whole) * 100 + 0.5f);
        writeInt(whole);
        buf[pos++] = '.';
        if (frac < 10) buf[pos++] = '0';
        writeInt(frac);
        };

    writeInt(id);       
    buf[pos++] = ',';
    writeStr(name);     
    buf[pos++] = ',';
    writeInt(age);      
    buf[pos++] = ',';
    buf[pos++] = gender[0]; 
    buf[pos++] = ',';
    writeStr(contact);   
    buf[pos++] = ',';
    writeStr(password); 
    buf[pos++] = ',';
    writeFloat(balance);
    buf[pos] = '\0';
}

//comma separated record line
Patient Patient::fromFileLine(const char* line) 
{
    // Copy line so we can tokenise manually
    char buf[512];
    int i = 0;
    while (line[i] != '\0' && i < 511) 
    {
        buf[i] = line[i]; ++i; 
    }
    buf[i] = '\0';

    // Manual tokeniser
    char* fields[7];
    int   fc = 0;
    fields[fc++] = buf;
    for (int j = 0; buf[j] != '\0' && fc < 7; ++j) 
    {
        if (buf[j] == ',') 
        {
            buf[j] = '\0';
            fields[fc++] = &buf[j + 1]; 
        }
    }
    if (fc < 7)
    {
        return Patient();
    }// malformed line

    // Parse fields
    int   pid = 0;  
    for (int j = 0; fields[0][j]; ++j)
    {
        pid = pid * 10 + (fields[0][j] - '0');
    }
    int   age = 0;  
    for (int j = 0; fields[2][j]; ++j)
    {
        age = age * 10 + (fields[2][j] - '0');
    }
    char  gender = fields[3][0];
    float bal = 0.f;
    {
        int   j = 0; 
        bool dec = false;
        float scale = 0.1f;
        while (fields[6][j]) 
        {
            if (fields[6][j] == '.') 
            {
                dec = true; 
                j++;
                continue; 
            }
            float d = (float)(fields[6][j] - '0');
            if (!dec)
            {
                bal = bal * 10.f + d;
            }
            else 
            {
                bal += d * scale; 
                scale *= 0.1f; 
            }
            ++j;
        }
    }
    return Patient(pid, fields[1], age, gender, fields[4], fields[5], bal);
}
void Patient::displayMenu() 
{ /* Handled by SFML GUI layer */ }
void Patient::saveToFile() 
{ /* Handled by FileHandler */ }