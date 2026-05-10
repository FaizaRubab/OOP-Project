#include "Prescription.h"
#include "Person.h"

Prescription::Prescription(): prescriptionId(0), appointmentId(0), patientId(0), doctorId(0) 
{
    date[0] = medicines[0] = notes[0] = '\0';
}

Prescription::Prescription(int pid, int aid, int patId, int docId,const char* d, const char* med, const char* n): prescriptionId(pid), appointmentId(aid), patientId(patId), doctorId(docId) 
{
    Person::strCopy(date, d, 12);
    Person::strCopy(medicines, med, 500);
    Person::strCopy(notes, n, 300);
}
Prescription::~Prescription() {}

int Prescription::getId() const 
{
    return prescriptionId; 
}
int Prescription::getAppointmentId() const 
{
    return appointmentId; 
}
int Prescription::getPatientId()const 
{
    return patientId; 
}
int Prescription::getDoctorId() const 
{
    return doctorId;
}
const char* Prescription::getDate() const 
{
    return date;
}
const char* Prescription::getMedicines() const 
{ 
    return medicines;
}
const char* Prescription::getNotes()const
{
    return notes;
}
void Prescription::toFileLine(char* buf, int maxLen) const 
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
    wInt(prescriptionId); 
    buf[pos++] = ',';
    wInt(appointmentId); 
    buf[pos++] = ',';
    wInt(patientId); 
    buf[pos++] = ',';
    wInt(doctorId);
    buf[pos++] = ',';
    wStr(date);
    buf[pos++] = ',';
    wStr(medicines);
    buf[pos++] = ',';
    wStr(notes);
    buf[pos] = '\0';
}
Prescription Prescription::fromFileLine(const char* line) 
{
    char buf[1024]; 
    int i = 0;
    while (line[i] && i < 1023) 
    {
        buf[i] = line[i];
        ++i;
    }
    buf[i] = '\0';
    char* f[7];
    int fc = 0;
    f[fc++] = buf;
    for (int j = 0; buf[j] && fc < 7; ++j)
        if (buf[j] == ',')
        {
            buf[j] = '\0';
            f[fc++] = &buf[j + 1];
        }
    if (fc < 7)
        return Prescription();
    auto toInt = [](const char* s)
    {
    int v = 0;
    for (int j = 0; s[j]; ++j) 
        v = v * 10 + (s[j] - '0');
    return v;
    };
    return Prescription(toInt(f[0]), toInt(f[1]), toInt(f[2]), toInt(f[3]),f[4], f[5], f[6]);
}