#include "Bill.h"
#include "Person.h"
#include <ctime>

Bill::Bill() : billId(0), patientId(0), appointmentId(0),amount(0.f), status(UNPAID)
{
    date[0] = '\0';
}
Bill::Bill(int bid, int pid, int aid, float amt, Status s, const char* d): billId(bid), patientId(pid), appointmentId(aid), amount(amt), status(s)
{
    Person::strCopy(date, d, 12);
}
Bill::~Bill() {}
int Bill::getId()const 
{
    return billId; 
}
int Bill::getPatientId()const 
{
    return patientId;
}
int Bill::getAppointmentId() const 
{
    return appointmentId;
}
float Bill::getAmount() const 
{
    return amount;
}
Bill::Status Bill::getStatus() const
{
    return status; 
}
const char* Bill::getDate() const
{
    return date;
}
void Bill::setStatus(Status s) 
{
    status = s; 
}
const char* Bill::getStatusStr() const 
{
    switch (status) 
    {
    case UNPAID:   
        return "unpaid";
    case PAID:   
        return "paid";
    case CANCELLED: 
        return "cancelled";
    }
    return "unknown";
}
Bill::Status Bill::parseStatus(const char* s) 
{
    if (Person::strEqual(s, "paid"))    
        return PAID;
    if (Person::strEqual(s, "cancelled")) 
        return CANCELLED;
    return UNPAID;
}
int Bill::daysFromToday() const
{
    //DD-MM-YYYY
    int dd = (date[0] - '0') * 10 + (date[1] - '0');
    int mm = (date[3] - '0') * 10 + (date[4] - '0');
    int yy = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    struct tm bt = {};
    bt.tm_mday = dd; 
    bt.tm_mon = mm - 1;
    bt.tm_year = yy - 1900;
    time_t billTime = mktime(&bt);
    time_t nowTime = time(nullptr);
    double diff = difftime(nowTime, billTime);
    return (int)(diff / 86400.0);
}
void Bill::toFileLine(char* buf, int maxLen) const 
{
    int pos = 0;
    auto wInt = [&](int v) 
    {
    char t[12]; 
    int ti = 0;
    if (v == 0) 
    {
        buf[pos++] = '0';
        return;
    }
    while (v > 0)
    {
        t[ti++] = '0' + v % 10; 
        v /= 10; 
    }
    for (int j = ti - 1; j >= 0 && pos < maxLen - 1; --j) 
        buf[pos++] = t[j];
    };
    auto wStr = [&](const char* s) 
    {
    for (int i = 0; s[i] && pos < maxLen - 1; ++i)
        buf[pos++] = s[i];
    };
    auto wFloat = [&](float f) 
    {
    int w = (int)f, fr = (int)((f - w) * 100 + 0.5f);
    wInt(w); 
    buf[pos++] = '.';
    if (fr < 10)
        buf[pos++] = '0';
    wInt(fr);
    };
    wInt(billId);  
    buf[pos++] = ',';
    wInt(patientId);   
    buf[pos++] = ',';
    wInt(appointmentId);
    buf[pos++] = ',';
    wFloat(amount);    
    buf[pos++] = ',';
    wStr(getStatusStr());
    buf[pos++] = ',';
    wStr(date);
    buf[pos] = '\0';
}

Bill Bill::fromFileLine(const char* line) 
{
    char buf[256]; 
    int i = 0;
    while (line[i] && i < 255) 
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
        return Bill();
    auto toInt = [](const char* s) 
    {
    int v = 0;
    for (int j = 0; s[j]; ++j)
        v = v * 10 + (s[j] - '0');
    return v;
    };
    float amt = 0.f;
    {
        int j = 0; 
        bool dec = false;
        float sc = 0.1f;
        while (f[3][j])
        {
            if (f[3][j] == '.') 
            {
                dec = true; 
                j++;
                continue;
            }
            float d = (float)(f[3][j] - '0');
            if (!dec) 
                amt = amt * 10.f + d; 
            else
            {
                amt += d * sc;
                sc *= 0.1f;
            }
            ++j;
        }
    }
    return Bill(toInt(f[0]), toInt(f[1]), toInt(f[2]), amt, parseStatus(f[4]), f[5]);
}