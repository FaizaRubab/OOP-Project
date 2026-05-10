#pragma once
class Prescription 
{
private:
    int prescriptionId;
    int appointmentId;
    int patientId;
    int doctorId;
    char date[12];
    char medicines[500];
    char notes[300];
public:
    Prescription();
    Prescription(int prescId, int apptId, int patId, int docId,const char* date, const char* medicines, const char* notes);
    ~Prescription();

    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId()const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;
    void toFileLine(char* buf, int maxLen) const;
    static Prescription fromFileLine(const char* line);
};