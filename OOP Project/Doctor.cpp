#include "Doctor.h"

Doctor::Doctor() : Person(), fee(0.f) 
{
    specialization[0] = '\0'; 
}
Doctor::Doctor(int id,const char* name,const char* spec,const char* contact,const char* password,float fee) : Person(id, name, contact, password),fee(fee) 
{
    strCopy(specialization, spec, 100);
}
Doctor::~Doctor() {}
const char* Doctor::getSpecialization() const 
{
    return specialization; 
}
float Doctor::getFee() const 
{
    return fee; 
}
int Doctor::getId() const
{
    return id;
}
//case-insensitive
bool Doctor::matchesSpecialization(const char* query) const 
{
    return strEqualIgnoreCase(specialization, query);
}
bool Doctor::operator==(const Doctor& other) const 
{
    return id == other.id;
}
std::ostream& operator<<(std::ostream& out, const Doctor& d) 
{
    out << "Doctor ID      : " << d.id << "\n"
        << "Name           : " << d.name << "\n"
        << "Specialization : " << d.specialization << "\n"
        << "Contact        : " << d.contact << "\n"
        << "Fee            : PKR " << d.fee;
    return out;
}

void Doctor::toFileLine(char* buf, int maxLen) const 
{
    // doctor_id,name,specialization,contact,password,fee
    int pos = 0;
    auto writeInt = [&](int v) 
        {
        char tmp[20];
        int ti = 0;
        if (v == 0) 
        {
            buf[pos++] = '0'; 
            return; 
        }
        while (v > 0) 
        {
            tmp[ti++] = '0' + v % 10; 
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
        int whole = (int)f, frac = (int)((f - whole) * 100 + 0.5f);
        writeInt(whole); 
        buf[pos++] = '.';
        if (frac < 10)
        {
            buf[pos++] = '0';
        }
        writeInt(frac);
        };

    writeInt(id);          
    buf[pos++] = ',';
    writeStr(name);          
    buf[pos++] = ',';
    writeStr(specialization); 
    buf[pos++] = ',';
    writeStr(contact);        
    buf[pos++] = ',';
    writeStr(password);       
    buf[pos++] = ',';
    writeFloat(fee);
    buf[pos] = '\0';
}
Doctor Doctor::fromFileLine(const char* line) 
{
    char buf[512]; 
    int i = 0;
    while (line[i] && i < 511)
    {
        buf[i] = line[i];
        ++i; 
    }
    buf[i] = '\0';

    char* f[6]; 
    int fc = 0; 
    f[fc++] = buf;
    for (int j = 0; buf[j] && fc < 6; ++j)
    {
        if (buf[j] == ',') 
        {
            buf[j] = '\0';
            f[fc++] = &buf[j + 1]; 
        }
    }
    if (fc < 6)
    {
        return Doctor();
    }
    int did = 0;
    for (int j = 0; f[0][j]; ++j)
    {
        did = did * 10 + (f[0][j] - '0');
    }
    float fee = 0.f;
    {
        int j = 0; 
        bool dec = false;
        float sc = 0.1f;
        while (f[5][j])
        {
            if (f[5][j] == '.') 
            {
                dec = true; 
                j++; 
                continue;
            }
            float d = (float)(f[5][j] - '0');
            if (!dec)
            {
                fee = fee * 10.f + d;
            }
            else
            {
                fee += d * sc; sc *= 0.1f; 
            }
            ++j;
        }
    }
    return Doctor(did, f[1], f[2], f[3], f[4], fee);
}
void Doctor::displayMenu() {}
void Doctor::saveToFile() {}