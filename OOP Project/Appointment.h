#pragma once
#include <ostream>

class Appointment 
{
public:
    // 8 fixed daily slots
    static const char* SLOTS[8];
    enum Status { PENDING, COMPLETED, CANCELLED, NOSHOW };

private:
    int appointmentId;
    int patientId;
    int doctorId;
    char date[12];       // DD-MM-YYYY + '\0'
    char timeSlot[6];    // HH:MM + '\0'
    Status status;
public:
    Appointment();
    Appointment(int,int,int,const char*,const char*,Status status = PENDING);
    ~Appointment();

    // Getters
    int getAppointmentId() const;
    int getId() const; 
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    Status getStatus() const;
    const char* getStatusStr() const;

    // Setters
    void setStatus(Status);

    //Operator overloads
    // Conflict: same doctor + same date + same slot + neither cancelled
    bool operator==(const Appointment&) const;
    friend std::ostream& operator<<(std::ostream&, const Appointment&);

    // Comparison for sorting by date ascending/descending
    int compareDateTo(const Appointment&) const;  // -1 / 0 / 1

    // File helpers
    void toFileLine(char*, int) const;
    static Appointment fromFileLine(const char*);

    // Validate slot string
    static bool isValidSlot(const char*);

    // Status string to enum
    static Status parseStatus(const char*);
};