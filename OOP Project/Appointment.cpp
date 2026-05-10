#include "Appointment.h"
#include "Person.h" 

const char* Appointment::SLOTS[8] ={"09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"};

Appointment::Appointment(): appointmentId(0), patientId(0), doctorId(0), status(PENDING) 
{
    date[0] = timeSlot[0] = '\0';
}

Appointment::Appointment(int apptId, int patId, int docId, const char* d, const char* t, Status s): appointmentId(apptId), patientId(patId), doctorId(docId), status(s) 
{
    Person::strCopy(date, d, 12);
    Person::strCopy(timeSlot, t, 6);
}

Appointment::~Appointment() {}

int Appointment::getAppointmentId() const 
{
    return appointmentId; 
}
int Appointment::getId() const
{
    return appointmentId; 
}
int Appointment::getPatientId() const 
{
    return patientId; 
}
int Appointment::getDoctorId() const
{
    return doctorId; 
}
const char* Appointment::getDate() const 
{
    return date; 
}
const char* Appointment::getTimeSlot() const
{
    return timeSlot;
}
Appointment::Status Appointment::getStatus() const
{
    return status;
}
void Appointment::setStatus(Status s) 
{
    status = s;
}
const char* Appointment::getStatusStr() const 
{
    switch (status) 
    {
        case PENDING:
        {
            return "pending";
        }
        case COMPLETED:
        {
            return "completed";
        }
        case CANCELLED:
        {
            return "cancelled";
        }
        case NOSHOW:
        {
            return "noshow";
        }
    }
    return "unknown";
}

bool Appointment::operator==(const Appointment& other) const 
{
    if (status == CANCELLED || other.status == CANCELLED)
    {
        return false;
    }
    return (doctorId == other.doctorId && Person::strEqual(date, other.date)&& Person::strEqual(timeSlot, other.timeSlot));
}

std::ostream& operator<<(std::ostream& out, const Appointment& a)
{
    out << "Appointment ID : " << a.appointmentId << "\n"
        << "Patient ID     : " << a.patientId << "\n"
        << "Doctor ID      : " << a.doctorId << "\n"
        << "Date           : " << a.date << "\n"
        << "Time Slot      : " << a.timeSlot << "\n"
        << "Status         : " << a.getStatusStr();
    return out;
}

// Compare dates in DD-MM-YYYY format; returns -1 / 0 / 1
int Appointment::compareDateTo(const Appointment& other) const 
{
    // Parse year, month, day manually
    auto parse = [](const char* d, int& dd, int& mm, int& yy) 
        {
        dd = (d[0] - '0') * 10 + (d[1] - '0');
        mm = (d[3] - '0') * 10 + (d[4] - '0');
        yy = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
        };
    int d1, m1, y1, d2, m2, y2;
    parse(date, d1, m1, y1);
    parse(other.date, d2, m2, y2);
    if (y1 != y2)
    {
        return y1 < y2 ? -1 : 1;
    }
    if (m1 != m2)
    {
        return m1 < m2 ? -1 : 1;
    }
    if (d1 != d2)
    {
        return d1 < d2 ? -1 : 1;
    }
    return 0;
}

bool Appointment::isValidSlot(const char* slot)
{
    for (int i = 0; i < 8; ++i)
    {
        if (Person::strEqual(slot, SLOTS[i])) 
            return true;
    }
    return false;
}

Appointment::Status Appointment::parseStatus(const char* s)
{
    if (Person::strEqual(s, "pending"))  
        return PENDING;
    if (Person::strEqual(s, "completed")) 
        return COMPLETED;
    if (Person::strEqual(s, "cancelled")) 
        return CANCELLED;
    return NOSHOW;
}

void Appointment::toFileLine(char* buf, int maxLen) const
{
    // appointment_id,patient_id,doctor_id,date,time_slot,status
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
        {
            buf[pos++] = t[j];
        }
        };
    auto wStr = [&](const char* s) 
        {
            for (int i = 0; s[i] && pos < maxLen - 1; ++i)
            {
                buf[pos++] = s[i];
            }
        };

    wInt(appointmentId); 
    buf[pos++] = ',';
    wInt(patientId);   
    buf[pos++] = ',';
    wInt(doctorId);     
    buf[pos++] = ',';
    wStr(date);          
    buf[pos++] = ',';
    wStr(timeSlot);    
    buf[pos++] = ',';
    wStr(getStatusStr());
    buf[pos] = '\0';
}

Appointment Appointment::fromFileLine(const char* line)
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
    {
        return Appointment();
    }
    auto toInt = [](const char* s) 
    {
        int v = 0; 
        for (int j = 0; s[j]; ++j)
        {
            v = v * 10 + (s[j] - '0');
            return v;
        }
    };

    return Appointment(toInt(f[0]), toInt(f[1]), toInt(f[2]),f[3], f[4], parseStatus(f[5]));
}