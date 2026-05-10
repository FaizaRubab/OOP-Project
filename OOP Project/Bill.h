#pragma once
class Bill 
{
public:
    enum Status { UNPAID, PAID, CANCELLED };
private:
    int billId;
    int patientId;
    int appointmentId;
    float amount;
    Status status;
    char date[12];     // DD-MM-YYYY
public:
    Bill();
    Bill(int,int,int,float,Status,const char*);
    ~Bill();

    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    Status getStatus() const;
    const char* getStatusStr() const;
    const char* getDate() const;

    void setStatus(Status);

    //for OVERDUE check
    int daysFromToday() const;

    void toFileLine(char*,int) const;
    static Bill fromFileLine(const char*);
    static Status parseStatus(const char*);
};