#pragma once
#ifdef _MSC_VER
#  pragma warning(disable : 4996)
#endif
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "HospitalException.h"

class FileHandler
{
public:
    //Load all entities
    static void loadPatients(Storage<Patient>&);
    static void loadDoctors(Storage<Doctor>&);
    static void loadAppointments(Storage<Appointment>&);
    static void loadBills(Storage<Bill>& store);
    static void loadPrescriptions(Storage<Prescription>&);
    static Admin loadAdmin();   // single admin record

    //Append new record to file
    static void appendPatient(const Patient&);
    static void appendDoctor(const Doctor&);
    static void appendAppointment(const Appointment&);
    static void appendBill(const Bill&);
    static void appendPrescription(const Prescription&);

    //Update record by ID (rewrites entire file)
    static void updatePatient(const Patient&);
    static void updateDoctor(const Doctor&);
    static void updateAppointment(const Appointment&);
    static void updateBill(const Bill&);

    //Delete record by ID
    static void deletePatient(int);
    static void deleteDoctor(int);

    //Discharge (archive + delete)
    static void archivePatient(const Patient& ,Storage<Appointment>&,Storage<Bill>&,Storage<Prescription>&);

    //Security log
    static void appendSecurityLog(const char*,const char*,const char*);

    //Read security log to buffer 
    static void readSecurityLog(char*,int);

private:
    // File path constants
    static const char* PATIENTS_FILE;
    static const char* DOCTORS_FILE;
    static const char* ADMIN_FILE;
    static const char* APPOINTMENTS_FILE;
    static const char* BILLS_FILE;
    static const char* PRESCRIPTIONS_FILE;
    static const char* SECURITY_LOG_FILE;
    static const char* DISCHARGED_FILE;

    // Helper: rewrite entire file from a storage
    template<typename T>
    static void rewriteFile(const char*,Storage<T>&);

    FileHandler() = delete;
};