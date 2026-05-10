#include "FileHandler.h"
#include <cstdio>
#include <ctime>

const char* FileHandler::PATIENTS_FILE = "patients.txt";
const char* FileHandler::DOCTORS_FILE = "doctors.txt";
const char* FileHandler::ADMIN_FILE = "admin.txt";
const char* FileHandler::APPOINTMENTS_FILE = "appointments.txt";
const char* FileHandler::BILLS_FILE = "bills.txt";
const char* FileHandler::PRESCRIPTIONS_FILE = "prescriptions.txt";
const char* FileHandler::SECURITY_LOG_FILE = "security_log.txt";
const char* FileHandler::DISCHARGED_FILE = "discharged.txt";

// ── Generic line-based file reader ─────────────────────────

template<typename T, typename ParseFn>
static void loadFile(const char* filename, Storage<T>& store, ParseFn parse) 
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        return;  // file may not exist on first run
    }
    char line[1024];
    while (fgets(line, 1024, f))
    {
        // Strip newline
        int len = 0;
        while (line[len])
        {
            ++len;
        }
        if (len > 0 && line[len - 1] == '\n')
        {
            line[--len] = '\0';
        }
        if (len > 0 && line[len - 1] == '\r')
        {
            line[--len] = '\0';
        }
        if (len == 0)
        {
            continue;
        }
        store.add(parse(line));
    }
    fclose(f);
}

//Load functions

void FileHandler::loadPatients(Storage<Patient>& store) 
{
    loadFile(PATIENTS_FILE, store, Patient::fromFileLine);
}
void FileHandler::loadDoctors(Storage<Doctor>& store) 
{
    loadFile(DOCTORS_FILE, store, Doctor::fromFileLine);
}
void FileHandler::loadAppointments(Storage<Appointment>& store) 
{
    loadFile(APPOINTMENTS_FILE, store, Appointment::fromFileLine);
}
void FileHandler::loadBills(Storage<Bill>& store) 
{
    loadFile(BILLS_FILE, store, Bill::fromFileLine);
}
void FileHandler::loadPrescriptions(Storage<Prescription>& store)
{
    loadFile(PRESCRIPTIONS_FILE, store, Prescription::fromFileLine);
}
Admin FileHandler::loadAdmin()
{
    FILE* f = fopen(ADMIN_FILE, "r");
    if (!f)
    {
        throw FileNotFoundException(ADMIN_FILE);
    }
    char line[256];
    Admin a;
    if (fgets(line, 256, f)) 
    {
        int len = 0; 
        while (line[len])
        {
            ++len;
        }
        if (len > 0 && line[len - 1] == '\n')
        {
            line[--len] = '\0';
        }
        a = Admin::fromFileLine(line);
    }
    fclose(f);
    return a;
}
static void appendLine(const char* filename, const char* line)
{
    FILE* f = fopen(filename, "a");
    if (!f)
    {
        return;
    }
    fputs(line, f);
    fputc('\n', f);
    fclose(f);
}
void FileHandler::appendPatient(const Patient& p) 
{
    char buf[512];
    p.toFileLine(buf, 512); 
    appendLine(PATIENTS_FILE, buf);
}
void FileHandler::appendDoctor(const Doctor& d)
{
    char buf[512]; 
    d.toFileLine(buf, 512);
    appendLine(DOCTORS_FILE, buf);
}
void FileHandler::appendAppointment(const Appointment& a) 
{
    char buf[256];
    a.toFileLine(buf, 256); 
    appendLine(APPOINTMENTS_FILE, buf);
}
void FileHandler::appendBill(const Bill& b) 
{
    char buf[256];
    b.toFileLine(buf, 256);
    appendLine(BILLS_FILE, buf);
}
void FileHandler::appendPrescription(const Prescription& p)
{
    char buf[1024];
    p.toFileLine(buf, 1024);
    appendLine(PRESCRIPTIONS_FILE, buf);
}
template<typename T>
static void rewriteStorage(const char* filename, Storage<T>& store) 
{
    FILE* f = fopen(filename, "w");
    if (!f)
    {
        return;
    }
    char buf[1024];
    for (int i = 0; i < store.size(); ++i)
    {
        store[i].toFileLine(buf, 1024);
        fputs(buf, f);
        fputc('\n', f);
    }
    fclose(f);
}
void FileHandler::updatePatient(const Patient& p) 
{
    Storage<Patient> store; 
    loadPatients(store);
    Patient* found = store.findById(p.getId());
    if (found) *found = p;
    rewriteStorage(PATIENTS_FILE, store);
}
void FileHandler::updateDoctor(const Doctor& d)
{
    Storage<Doctor> store; 
    loadDoctors(store);
    Doctor* found = store.findById(d.getId());
    if (found)
    {
        *found = d;
    }
    rewriteStorage(DOCTORS_FILE, store);
}
void FileHandler::updateAppointment(const Appointment& a) 
{
    Storage<Appointment> store;
    loadAppointments(store);
    Appointment* found = store.findById(a.getAppointmentId());
    if (found)
    {
        *found = a;
    }
    rewriteStorage(APPOINTMENTS_FILE, store);
}

void FileHandler::updateBill(const Bill& b)
{
    Storage<Bill> store; 
    loadBills(store);
    for (int i = 0; i < store.size(); ++i) 
    {
        if (store[i].getId() == b.getId()) 
        {
            store[i] = b; break;
        }
    }
    rewriteStorage(BILLS_FILE, store);
}
void FileHandler::deletePatient(int id) 
{
    Storage<Patient> store;
    loadPatients(store);
    store.removeById(id);
    rewriteStorage(PATIENTS_FILE, store);
}

void FileHandler::deleteDoctor(int id) 
{
    Storage<Doctor> store;
    loadDoctors(store);
    store.removeById(id);
    rewriteStorage(DOCTORS_FILE, store);
}
void FileHandler::archivePatient(const Patient& p,Storage<Appointment>& appts,Storage<Bill>& bills,Storage<Prescription>& prescs) {
    FILE* f = fopen(DISCHARGED_FILE, "a");
    if (f) 
    {
        char buf[1024];
        // Patient row
        p.toFileLine(buf, 512);
        fputs(buf, f);
        fputc('\n', f);
        // Appointments
        for (int i = 0; i < appts.size(); ++i)
        {
            if (appts[i].getPatientId() == p.getId())
            {
                appts[i].toFileLine(buf, 256); 
                fputs(buf, f);
                fputc('\n', f);
            }
        }
        // Bills
        for (int i = 0; i < bills.size(); ++i)
        {
            if (bills[i].getPatientId() == p.getId())
            {
                bills[i].toFileLine(buf, 256); 
                fputs(buf, f);
                fputc('\n', f);
            }
        }
        // Prescriptions
        for (int i = 0; i < prescs.size(); ++i)
        {
            if (prescs[i].getPatientId() == p.getId())
            {
                prescs[i].toFileLine(buf, 1024); 
                fputs(buf, f);
                fputc('\n', f);
            }
        }
        fclose(f);
    }
    deletePatient(p.getId());

    // Remove appointments
    Storage<Appointment> newAppts; 
    loadAppointments(newAppts);
    for (int i = newAppts.size() - 1; i >= 0; --i)
    {
        if (newAppts[i].getPatientId() == p.getId()) newAppts.removeAt(i);
    }
    rewriteStorage(APPOINTMENTS_FILE, newAppts);

    // Remove bills
    Storage<Bill> newBills; loadBills(newBills);
    for (int i = newBills.size() - 1; i >= 0; --i)
    {
        if (newBills[i].getPatientId() == p.getId()) newBills.removeAt(i);
    }
    rewriteStorage(BILLS_FILE, newBills);

    // Remove prescriptions
    Storage<Prescription> newPrescs; loadPrescriptions(newPrescs);
    for (int i = newPrescs.size() - 1; i >= 0; --i)
    {
        if (newPrescs[i].getPatientId() == p.getId()) newPrescs.removeAt(i);
    }
    rewriteStorage(PRESCRIPTIONS_FILE, newPrescs);
}
void FileHandler::appendSecurityLog(const char* role,const char* enteredId,const char* result)
{
    // Get timestamp using ctime/strftime
    time_t now = time(nullptr);
    struct tm* lt = localtime(&now);
    char ts[32];
    strftime(ts,32,"%d-%m-%Y %H:%M:%S",lt);

    // Compose log line
    char line[512];
    int pos = 0;
    auto wStr = [&](const char* s)
    {
            for (int i = 0; s[i] && pos < 511; ++i)
            {
                line[pos++] = s[i];
            }
    };
    wStr(ts);      
    line[pos++] = ',';
    wStr(role);    
    line[pos++] = ',';
    wStr(enteredId);
    line[pos++] = ',';
    wStr(result);
    line[pos] = '\0';

    appendLine(SECURITY_LOG_FILE, line);
}

void FileHandler::readSecurityLog(char* buf, int maxLen)
{
    FILE* f = fopen(SECURITY_LOG_FILE, "r");
    if (!f)
    {
        buf[0] = '\0'; 
        return;
    }
    int pos = 0;
    int c;
    while ((c = fgetc(f)) != EOF && pos < maxLen - 1)
    {
        buf[pos++] = (char)c;
    }
    buf[pos] = '\0';
    fclose(f);
}