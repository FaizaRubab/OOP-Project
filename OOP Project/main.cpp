#ifdef _MSC_VER
#  pragma warning(disable : 4996)   // fopen, localtime, strftime
#  pragma warning(disable : 4101)   // unreferenced local variable
#endif

#include <SFML/Graphics.hpp>
using namespace sf;
#include <SFML/Window.hpp>
#include <ctime>

#include "Person.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"

//  Colour palette
static const Color background(115, 115, 115);
static const Color backgroundPanel(0,0,0);
static const Color cardBox(191, 191, 191);
static const Color input(0,0,0);
static const Color patientBar(0, 102, 102);
static const Color doctorBar(102, 0, 0);
static const Color cancelButton(255,255,255);
static const Color adminBar(0, 0, 102);
static const Color dischargeButton(52, 211, 153);
static const Color text(255,255,255);
static const Color captionButton(0,0,0);
static const Color border(203, 213, 225);

//  Enums
enum class Screen 
{
    roleSelect,login,patientMenu,doctorMenu,adminMenu,
    // Patient
    bookAppointment,cancelAppointment,viewAppointment,viewRecords,viewBill,payBill,topUp,
    // Doctor
    doctorToday,doctorMark,doctorPrescription,doctorHistory,
    // Admin
    adminAddDoctor,adminViewAll,adminDischarge,adminSecurity,adminReport,
    // Shared
    messageBox
};
enum class Role { NONE, PATIENT, DOCTOR, ADMIN };
static void IntegerToCharArray(int v, char* buf) 
{
    if (v == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    char temp[20];
    int tempIndex = 0;
    bool negative = false;
    if (v < 0)
    {
        negative = true;
        v = -v;
    }
    while (v > 0)
    {
        int digit = v % 10;
        char character = '0' + digit;
        temp[tempIndex] = character;
        tempIndex = tempIndex + 1;
        v = v / 10;
    }
    int bufferIndex = 0;
    if (negative == true)
    {
        buf[bufferIndex] = '-';
        bufferIndex = bufferIndex + 1;
    }
    // Reverse copy bcz when we take %10 it saves last digit first
    for (int i = tempIndex - 1; i >= 0; i--)
    {
        buf[bufferIndex] = temp[i];
        bufferIndex = bufferIndex + 1;
    }
    buf[bufferIndex] = '\0';
}
static void ConvertFloatToString(float number, char* result) 
{
    int wholePart = (int)number;
    float decimalValue = number - wholePart;                        // Separate decimal part from int
    int fractionalPart = (int)(decimalValue * 100 + 0.5f);          // Convert decimal to 2 digits
    char wholeBuffer[20];                           // TempArrays
    char fractionalBuffer[10];
    IntegerToCharArray(wholePart, wholeBuffer);                      // Convert integer parts into strings
    IntegerToCharArray(fractionalPart, fractionalBuffer);
    int resultIndex = 0;
    int i = 0;
    while (wholeBuffer[i] != '\0')
    {
        result[resultIndex] = wholeBuffer[i];
        resultIndex = resultIndex + 1;
        i = i + 1;
    }
    result[resultIndex] = '.';             // Add decimal point
    resultIndex = resultIndex + 1;
    // If decimal is single digit
    if (fractionalPart < 10)
    {
        result[resultIndex] = '0';
        resultIndex = resultIndex + 1;
    }
    // Copy decimal digits
    i = 0;
    while (fractionalBuffer[i] != '\0')
    {
        result[resultIndex] = fractionalBuffer[i];
        resultIndex = resultIndex + 1;
        i = i + 1;
    }
    result[resultIndex] = '\0';
}
static void sapp(char* dst, const char* src, int cap)
{
    int dstIndex = 0;
    while (dst[dstIndex] != '\0')
    {
        dstIndex = dstIndex + 1;
    }
    int srcIndex = 0;
    while (src[srcIndex] != '\0' && dstIndex < cap - 1)
    {
        dst[dstIndex] = src[srcIndex];
        dstIndex = dstIndex + 1;
        srcIndex = srcIndex + 1;
    }
    dst[dstIndex] = '\0';
}

//  UiButton
struct UiButton
{
    RectangleShape rect;
    Text lbl;
    Color cn, ch;
    bool hov = false;

    float posX, posY;
    float width, height;

    void init(float x, float y, float w, float h, const char* txt, Font& font, Color a = Color(20, 184, 166),Color b = Color(17, 224, 204))
    {
        cn = a;
        ch = b;

        posX = x;
        posY = y;
        width = w;
        height = h;

        rect.setPosition(x, y);
        rect.setSize(Vector2f(w, h));
        rect.setFillColor(a);

        lbl.setFont(font);
        lbl.setString(txt);
        lbl.setCharacterSize(15);
        lbl.setFillColor(Color(255,255,255));

        FloatRect lb = lbl.getLocalBounds();

        lbl.setOrigin(lb.left + lb.width / 2.f,lb.top + lb.height / 2.f);
        lbl.setPosition(x + w / 2.f, y + h / 2.f);
    }
    void update(float mouseX, float mouseY)
    {
        hov = (mouseX >= posX && mouseX <= posX + width && mouseY >= posY && mouseY <= posY + height);
        rect.setFillColor(hov ? ch : cn);
    }
    void draw(RenderWindow& w)
    {
        w.draw(rect);
        w.draw(lbl);
    }
    bool hit(Vector2f pt)
    {
        return rect.getGlobalBounds().contains(pt);
    }
};
//  UiInput
struct UiInput
{
    RectangleShape rect;
    Text caption;
    Text display;
    char buffer[256];
    int length;
    bool focused;
    bool isPassword;
    float posX;
    float posY;
    float width;
    float height;
    void init(float x, float y, float w,const char* cap,Font& font,bool pw = false)
    {
        posX = x;
        posY = y;
        width = w;
        height = 38.f;

        focused = false;
        isPassword = pw;
        length = 0;
        buffer[0] = '\0';
        
        caption.setFont(font);           // Caption
        caption.setString(cap);
        caption.setCharacterSize(12);
        caption.setFillColor(captionButton);
        caption.setPosition(x, y);

        // Input box
        rect.setPosition(x, y + 18.f);
        rect.setSize(Vector2f(w, 38.f));
        rect.setFillColor(input);
        rect.setOutlineThickness(1.5f);
        rect.setOutlineColor(border);
        // Text display
        display.setFont(font);
        display.setCharacterSize(14);
        display.setFillColor(text);
        display.setPosition(x + 8.f, y + 26.f);
    }
    void onChar(char c)
    {
        if (focused == false)
        {
            return;
        }
        // Backspace
        if (c == '\b')
        {
            if (length > 0)
            {
                length = length - 1;

                buffer[length] = '\0';
            }
        }
        else
        {
            // Printable characters
            if (c >= 32 && length < 254)
            {
                buffer[length] = c;
                length = length + 1;
                buffer[length] = '\0';
            }
        }
    }
    void update(float mouseX, float mouseY, bool mousePressed)
    {
        if (mousePressed == true)
        {
            bool insideX =(mouseX >= posX &&mouseX <= posX + width);
            bool insideY = (mouseY >= posY + 18.f && mouseY <= posY + 18.f + height);
            focused = insideX && insideY;
        }
    }
    void draw(RenderWindow& window)
    {
        // Border color
        if (focused == true)
        {
            rect.setOutlineColor(patientBar);
        }
        else
        {
            rect.setOutlineColor(border);
        }
        window.draw(rect);
        window.draw(caption);
        // Password display
        if (isPassword == true)
        {
            char hidden[256];
            for (int i = 0; i < length; i = i + 1)
            {
                hidden[i] = '*';
            }
            hidden[length] = '\0';
            display.setString(hidden);
        }
        else
        {
            display.setString(buffer);
        }
        window.draw(display);
    }
    void clear()
    {
        length = 0;
        buffer[0] = '\0';
    }
};

//  Global state
static Storage<Patient> gP;                // patients
static Storage<Doctor> gD;                // doctors
static Storage<Appointment> gA;          // appointments
static Storage<Bill> gB;                // bills
static Storage<Prescription> gRx;      // prescriptions
static Admin gAdmin;

static Screen gScreen = Screen::roleSelect;
static Screen gRetScreen = Screen::roleSelect;
static Role gRole = Role::NONE;
static int gUid = -1;
static int gFails = 0;
static char gMsg[512] = {};
static char gList[10240] = {};
static float gScroll = 0.f;

//  List helpers
static void lClear()
{
    gList[0] = '\0';
}
static void lAdd(const char* s)
{
    sapp(gList, s, 10240);
}
//bubble sort ascending date
static void sortAscDate(Appointment* a, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (a[j].compareDateTo(a[j + 1]) > 0) 
            {
                Appointment t = a[j]; 
                a[j] = a[j + 1]; 
                a[j + 1] = t;
            }
        }
    }
}
// Descending date
static void sortDescDate(Appointment* a, int n) 
{
    for (int i = 0; i < n - 1; ++i)
    { 
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (a[j].compareDateTo(a[j + 1]) < 0) 
            {
                Appointment t = a[j]; 
                a[j] = a[j + 1]; 
                a[j + 1] = t;
            }
        }
    }
}

static void buildMyAppointments()
{
    lClear();          // Clear previous list
    lAdd("ID   | Date        | Slot  | Status     | Doctor\n");        // Table headings
    lAdd("-----+-------------+-------+------------+--------------------\n");
    Appointment tempAppointments[100];
    int totalCount = 0;
    
    for (int i = 0; i < gA.size(); i = i + 1)                // Collect current user's appointments
    {
        if (gA[i].getPatientId() == gUid)
        {
            tempAppointments[totalCount] = gA[i];
            totalCount = totalCount + 1;
        }
    }
    sortAscDate(tempAppointments, totalCount);             // Sort by date
    for (int i = 0; i < totalCount; i = i + 1)            // Display
    {
        char line[256];
        line[0] = '\0';
        char idBuffer[20];
        IntegerToCharArray(tempAppointments[i].getAppointmentId(),idBuffer);
        sapp(line, idBuffer, 256);                                   //id
        sapp(line, " | ", 256);
       
        sapp(line, tempAppointments[i].getDate(),256);              // Date
        sapp(line, " | ", 256);
       
        sapp(line,tempAppointments[i].getTimeSlot(),256);           // Time slot
        sapp(line, " | ", 256);

        sapp(line,tempAppointments[i].getStatusStr(),256);          // Status
        sapp(line, " | ", 256);

        Doctor* doctor =gD.findById(tempAppointments[i].getDoctorId());         // Find doctor
        
        if (doctor != NULL)                                   // Add doctor name
        {
            sapp(line,doctor->getName(),256);
        }
        else
        {
            sapp(line, "Unknown", 256);
        }
        sapp(line, "\n", 256);                                 //new line
        lAdd(line);                                           // Add to list
    }
    if (totalCount == 0)
    {
        lAdd("No appointments found.\n");
    }
}
static void buildMyBills(bool unpaidOnly)
{
    lClear();
    lAdd("Bill | Appt | Amount (PKR) | Status     | Date\n");
    lAdd("-----+------+--------------+------------+-----------\n");
    float totalOutstanding = 0.0f;
    int totalBills = 0;
   
    for (int i = 0; i < gB.size(); i = i + 1)                        // Check all bills
    {
        if (gB[i].getPatientId() != gUid)
        {
            continue;
        }
        if (unpaidOnly == true)
        {
            if (gB[i].getStatus() != Bill::UNPAID)
            {
                continue;
            }
        }
        totalBills = totalBills + 1;
        char line[256];
        line[0] = '\0';
        char billIdBuffer[20];
        IntegerToCharArray(gB[i].getId(),billIdBuffer);
        sapp(line, billIdBuffer, 256);
        sapp(line, " | ", 256);
        
        char appointmentIdBuffer[20];
        IntegerToCharArray(gB[i].getAppointmentId(),appointmentIdBuffer);
        sapp(line, appointmentIdBuffer, 256);
        sapp(line, " | ", 256);
        
        char amountBuffer[20];
        ConvertFloatToString(gB[i].getAmount(),amountBuffer);
        sapp(line, amountBuffer, 256);
        sapp(line, " | ", 256);
        
        sapp(line,gB[i].getStatusStr(),256);
        sapp(line, " | ", 256);

        sapp(line,gB[i].getDate(),256);
        sapp(line, "\n", 256);
        lAdd(line);
        
        if (gB[i].getStatus() == Bill::UNPAID)             // Calculate unpaid total
        {
            totalOutstanding =totalOutstanding +gB[i].getAmount();
        }
    }
    if (totalBills == 0)
    {
        lAdd("No bills found.\n");
        return;
    }
    char totalBuffer[20];
    ConvertFloatToString(totalOutstanding,totalBuffer);
    
    lAdd("\nTotal outstanding (PKR): ");
    lAdd(totalBuffer);
    lAdd("\n");
}
static void buildAllPatients()
{
    lClear();
    lAdd("ID  | Name                 | Age | Balance (PKR) | Unpaid\n");
    lAdd("----+----------------------+-----+---------------+-------\n");
    for (int i = 0; i < gP.size(); i = i + 1)
    {
        int unpaidBills = 0;
        for (int j = 0; j < gB.size(); j = j + 1)                        // Count unpaid bills
        {
            bool samePatient =(gB[j].getPatientId() == gP[i].getId());
            bool unpaid =(gB[j].getStatus() == Bill::UNPAID);
            if (samePatient && unpaid)
            {
                unpaidBills = unpaidBills + 1;
            }
        }
        char line[256];
        line[0] = '\0';
        char idBuffer[20];                      
        IntegerToCharArray(gP[i].getId(),idBuffer);
        sapp(line, idBuffer, 256);
        sapp(line, " | ", 256);

        sapp(line, gP[i].getName(), 256);
        sapp(line, " | ", 256);

        char ageBuffer[20];
        IntegerToCharArray(gP[i].getAge(),ageBuffer);
        sapp(line, ageBuffer, 256);
        sapp(line, " | ", 256);
        
        char balanceBuffer[20];
        ConvertFloatToString(gP[i].getBalance(),balanceBuffer);
        sapp(line, balanceBuffer, 256);
        sapp(line, " | ", 256);

        char unpaidBuffer[20];
        IntegerToCharArray(unpaidBills,unpaidBuffer);
        sapp(line, unpaidBuffer, 256);
        sapp(line, "\n", 256);
        
        lAdd(line);
    }
    if (gP.size() == 0)
    {
        lAdd("No patients registered.\n");
    }
}
static void buildAllDoctors()
{
    lClear();
    lAdd("ID  | Name                 | Specialization       | Fee\n");
    lAdd("----+----------------------+----------------------+---------\n");
    for (int i = 0; i < gD.size(); i = i + 1)
    {
        char line[256];
        line[0] = '\0';

        char idBuffer[20];
        IntegerToCharArray(gD[i].getId(),idBuffer);
        sapp(line, idBuffer, 256);
        sapp(line, " | ", 256);

        sapp(line,gD[i].getName(),256);
        sapp(line, " | ", 256);

        sapp(line,gD[i].getSpecialization(),256);
        sapp(line, " | ", 256);

        char feeBuffer[20];
        ConvertFloatToString(gD[i].getFee(),feeBuffer);
        sapp(line, feeBuffer, 256);
        sapp(line, "\n", 256);

        lAdd(line);
    }
    if (gD.size() == 0)
    {
        lAdd("No doctors registered.\n");
    }
}
static void buildAllAppointments()
{
    lClear();
    lAdd("ID  | Patient         | Doctor          | Date       | Slot  | Status\n");
    lAdd("----+-----------------+-----------------+------------+-------+-----------\n");

    Appointment tempAppointments[100];
    int totalCount = 0;

    for (int i = 0; i < gA.size() && totalCount < 100; i = i + 1)
    {
        tempAppointments[totalCount] = gA[i];
        totalCount = totalCount + 1;
    }
    sortDescDate(tempAppointments, totalCount);            // Sort appointments
    for (int i = 0; i < totalCount; i = i + 1)
    {
        char line[256];
        line[0] = '\0';
        char idBuffer[20];

        IntegerToCharArray(tempAppointments[i].getAppointmentId(),idBuffer);
        sapp(line, idBuffer, 256);
        sapp(line, " | ", 256);

        Patient* patient =gP.findById(tempAppointments[i].getPatientId());            // Find patient
        Doctor* doctor = gD.findById(tempAppointments[i].getDoctorId());               // Find doctor
        
        if (patient != NULL)                           // Patient name
        {
            sapp(line,patient->getName(),256);
        }
        else
        {
            sapp(line, "Unknown", 256);
        }
        sapp(line, " | ", 256);

        if (doctor != NULL)                     // Doctor name
        {
            sapp(line,doctor->getName(),256);
        }
        else
        {
            sapp(line, "Unknown", 256);
        }
        sapp(line, " | ", 256);

        sapp(line,tempAppointments[i].getDate(),256);
        sapp(line, " | ", 256);

        sapp(line,tempAppointments[i].getTimeSlot(),256);
        sapp(line, " | ", 256);

        sapp(line,tempAppointments[i].getStatusStr(),256);
        sapp(line, "\n", 256);

        lAdd(line);
    }
    if (totalCount == 0)
    {
        lAdd("No appointments found.\n");
    }
}
static void buildUnpaidBills()
{
    lClear();
    lAdd("Bill | Patient          | Amount (PKR) | Date       | Note\n");
    lAdd("-----+------------------+--------------+------------+----------\n");

    bool found = false;
    for (int i = 0; i < gB.size(); i = i + 1)
    {
        if (gB[i].getStatus() != Bill::UNPAID)
        {
            continue;
        }
        found = true;
        char line[256];
        line[0] = '\0';

        char billIdBuffer[20];
        IntegerToCharArray(gB[i].getId(),billIdBuffer);
        sapp(line, billIdBuffer, 256);
        sapp(line, " | ", 256);

        Patient* patient =gP.findById(gB[i].getPatientId());
        if (patient != NULL)
        {
            sapp(line, patient->getName(),256);
        }
        else
        {
            sapp(line, "Unknown", 256);
        }
        sapp(line, " | ", 256);

        char amountBuffer[20];
        ConvertFloatToString(gB[i].getAmount(),amountBuffer);
        sapp(line, amountBuffer, 256);
        sapp(line, " | ", 256);
        
        sapp(line,gB[i].getDate(),256);
        sapp(line, " | ", 256);

        if (gB[i].daysFromToday() > 7)
        {
            sapp(line, "[OVERDUE]", 256);
        }
        sapp(line, "\n", 256);
        lAdd(line);
    }
    if (found == false)
    {
        lAdd("No unpaid bills.\n");
    }
}
static void buildDailyReport()
{
    lClear();
    time_t currentTime = time(NULL);
    tm* localTime = localtime(&currentTime);
    char today[12];
    strftime(today, 12, "%d-%m-%Y", localTime);
    
    lAdd("Daily Report - ");                // Title
    lAdd(today);
    lAdd("\n");
    lAdd("================================================\n");
    int totalAppointments = 0;
    int pendingAppointments = 0;
    int completedAppointments = 0;
    int noShowAppointments = 0;
    int cancelledAppointments = 0;
    float revenue = 0.0f;
    
    for (int i = 0; i < gA.size(); i = i + 1)
    {
        if (Person::strEqual(gA[i].getDate(), today) == false)
        {
            continue;
        }

        totalAppointments =totalAppointments + 1;
        switch (gA[i].getStatus())
        {
            case Appointment::PENDING:
            {
                pendingAppointments = pendingAppointments + 1;
                break;
            }
            case Appointment::COMPLETED:
            {
                completedAppointments = completedAppointments + 1;
                break;
            }
            case Appointment::NOSHOW:
            {
                noShowAppointments = noShowAppointments + 1;
                break;
            }
            case Appointment::CANCELLED:
            {
                cancelledAppointments =cancelledAppointments + 1;
                break;
            }
        }
    }
    // Revenue
    for (int i = 0; i < gB.size(); i = i + 1)
    {
        bool sameDate =Person::strEqual(gB[i].getDate(),today);
        bool paid =(gB[i].getStatus() == Bill::PAID);
        if (sameDate && paid)
        {
            revenue =revenue +gB[i].getAmount();
        }
    }
    char numberBuffer[20];
    IntegerToCharArray(totalAppointments,numberBuffer);
    lAdd("Total appointments : ");
    lAdd(numberBuffer);
    lAdd("\n");

    // Pending
    IntegerToCharArray(pendingAppointments,numberBuffer);
    lAdd("Pending appointments : ");
    lAdd(numberBuffer);
    lAdd("\n");

    // Completed
    IntegerToCharArray(completedAppointments,numberBuffer);
    lAdd("Completed appointments : ");
    lAdd(numberBuffer);
    lAdd("\n");

    // No-show
    IntegerToCharArray(noShowAppointments,numberBuffer);
    lAdd("No-show appointments : ");
    lAdd(numberBuffer);
    lAdd("\n");

    // Cancelled
    IntegerToCharArray(cancelledAppointments,numberBuffer);
    lAdd("Cancelled appointments : ");
    lAdd(numberBuffer);
    lAdd("\n");

    // Revenue
    char revenueBuffer[20];
    ConvertFloatToString(revenue,revenueBuffer);
    lAdd("Revenue (PKR) : ");
    lAdd(revenueBuffer);
    lAdd("\n\n");

    // Doctor summary
    lAdd("Doctor summary (today):\n");
    lAdd("Doctor              | Comp | Pend | No-show\n");
    lAdd("--------------------+------+------+--------\n");

    for (int doctorIndex = 0; doctorIndex < gD.size();doctorIndex = doctorIndex + 1)
    {
        int completed = 0;
        int pending = 0;
        int noShow = 0;
        for (int i = 0; i < gA.size(); i = i + 1)           // Appointment loop
        {
            bool sameDoctor =(gA[i].getDoctorId() ==gD[doctorIndex].getId());
            bool sameDate =Person::strEqual(gA[i].getDate(),today);
            if (sameDoctor == false || sameDate == false)
            {
                continue;
            }
            switch (gA[i].getStatus())
            {
                case Appointment::COMPLETED:
                {
                    completed = completed + 1;
                    break;
                }
                case Appointment::PENDING:
                {
                    pending = pending + 1;
                    break;
                }
                case Appointment::NOSHOW:
                {
                    noShow = noShow + 1;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        if (completed + pending + noShow == 0)
        {
            continue;
        }
        char line[128];
        line[0] = '\0';
        sapp(line,gD[doctorIndex].getName(),128);
        sapp(line, " | ", 128);
       
        char valueBuffer[20];
        IntegerToCharArray(completed,valueBuffer);
        sapp(line, valueBuffer, 128);
        sapp(line, " | ", 128);

        IntegerToCharArray(pending,valueBuffer);
        sapp(line, valueBuffer, 128);
        sapp(line, " | ", 128);

        IntegerToCharArray(noShow,valueBuffer);
        sapp(line, valueBuffer, 128);
        sapp(line, "\n", 128);
        
        lAdd(line);
    }
}

// Draw helpers

static Font* gFont = NULL;
static void drawText( RenderWindow& window,const char* text,float x,float y,unsigned size = 14,Color color = Color(236, 237, 248))       // Draw text
{
    Text displayText;
    displayText.setFont(*gFont);
    displayText.setString(text);
    displayText.setCharacterSize(size);
    displayText.setFillColor(color);
    displayText.setPosition(x, y);

    window.draw(displayText);
}
static void drawCenterTitle(RenderWindow& window,const char* text,float y,unsigned size = 30,Color color = Color(236, 237, 248))      // Draw title
{
    Text title;
    title.setFont(*gFont);
    title.setString(text);
    title.setCharacterSize(size);
    title.setFillColor(color);

    FloatRect bounds =title.getLocalBounds();
    float centerX =(1280.0f - bounds.width) / 2.0f;
    title.setPosition(centerX, y);

    window.draw(title);
}
static void drawCard(RenderWindow& window,float x,float y,float cardWidth,float cardHeight)          // Draw card box
{
    RectangleShape card;
    card.setSize(Vector2f(cardWidth, cardHeight));
    card.setPosition(x, y);
    card.setFillColor(cardBox);
    card.setOutlineColor(border);
    card.setOutlineThickness(1.0f);

    window.draw(card);
}
static void drawScrollList(RenderWindow& window,float x,float y,float panelWidth,float panelHeight)         // Draw scrollable list
{
    RectangleShape panel;
    panel.setSize(Vector2f(panelWidth, panelHeight));
    panel.setPosition(x, y);
    panel.setFillColor(backgroundPanel);
    panel.setOutlineColor(border);
    panel.setOutlineThickness(1.0f);

    window.draw(panel);

    float lineHeight = 19.0f;
    float currentY =y + 6.0f - gScroll;
    const char* pointer = gList;                 // Start reading list
    
    while (*pointer != '\0')
    {
        const char* newLinePointer = pointer;
        while (*newLinePointer != '\0' && *newLinePointer != '\n')
        {
            newLinePointer = newLinePointer + 1;
        }
        char lineBuffer[512];
        int length =(int)(newLinePointer - pointer);
        if (length > 510)
        {
            length = 510;
        }
        for (int i = 0; i < length; i = i + 1)
        {
            lineBuffer[i] = pointer[i];
        }
        lineBuffer[length] = '\0';
        bool visibleTop =(currentY > y - lineHeight);
        bool visibleBottom =(currentY < y + panelHeight);

        if (visibleTop && visibleBottom)
        {
            Text lineText;
            lineText.setFont(*gFont);
            lineText.setString(lineBuffer);
            lineText.setCharacterSize(13);
            lineText.setFillColor(text);
            lineText.setPosition(x + 10.0f,currentY);

            window.draw(lineText);
        }
        currentY =currentY + lineHeight;
        
        if (*newLinePointer == '\n')             // Move pointer to next line
        {
            pointer = newLinePointer + 1;
        }
        else
        {
            pointer = newLinePointer;
        }
    }
}
int main() 
{
    FileHandler::loadPatients(gP);
    FileHandler::loadDoctors(gD);
    FileHandler::loadAppointments(gA);
    FileHandler::loadBills(gB);
    FileHandler::loadPrescriptions(gRx);
    try 
    {
        gAdmin = FileHandler::loadAdmin(); 
    }
    catch (FileNotFoundException&)
    {
        gAdmin = Admin(1, "Admin", "admin123"); 
    }
    RenderWindow window(VideoMode(1280, 800),"MediCore Hospital Management System, Lahore",Style::Close |Style::Titlebar);
    window.setFramerateLimit(60);
    Font font;
    if (!font.loadFromFile("assets/ARIAL.TTF"))
    {}
    gFont = &font;
    static UiButton btn[16];
    static UiInput  inp[6];
    int bc = 0, ic = 0;  // active counts
    auto resetUI = [&]() 
    {
            bc = 0;
            ic = 0; 
            gScroll = 0.f; 
    };
    auto mkBackBtn = [&]()
    {
        resetUI();
        btn[0].init(490, 700, 300, 46, "< Back to Menu", font,Color(50, 55, 80),Color(70, 75, 100));
        bc = 1;
    };
    auto buildRoleSelect = [&]() 
    {
        resetUI();
        btn[0].init(490, 240, 300, 54, "Login as Patient", font, patientBar,Color(17, 224, 204));
        btn[1].init(490, 310, 300, 54, "Login as Doctor", font, doctorBar,Color(79, 160, 255));
        btn[2].init(490, 380, 300, 54, "Login as Admin", font, adminBar,Color(255, 215, 80));
        btn[3].init(490, 470, 300, 46, "Exit", font,Color(50, 55, 80),Color(70, 75, 100));
        bc = 4;
    };
    auto buildLogin = [&]() 
    {
        resetUI();
        inp[0].init(440, 200, 400, "Enter your ID:", font);
        inp[1].init(440, 290, 400, "Enter your Password:", font, true);
        const char* lbl = (gRole == Role::PATIENT) ? "Login as Patient":(gRole == Role::DOCTOR) ? "Login as Doctor" : "Login as Admin";
        btn[0].init(440, 390, 185, 46, lbl, font);
        btn[1].init(645, 390, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
        bc = 2; 
        ic = 2;
    };
    auto buildPatMenu = [&]() 
    {
        resetUI();
        const char* L[8] = {"1. Book Appointment", 
                            "2. Cancel Appointment",
                            "3. View Appointments", 
                            "4. Medical Records",
                            "5. View Bills",     
                            "6. Pay Bill",
                            "7. Top Up Balance",   
                            "8. Logout"};
        for (int i = 0; i < 8; ++i) 
        {
            float c = (float)(i % 2), r = (float)(i / 2);
            Color cn = (i == 7) ? Color(50, 55, 80) : patientBar;
            Color ch = (i == 7) ? Color(70, 75, 100) : Color(17, 224, 204);
            btn[i].init(120.f + c * 560.f, 190.f + r * 88.f, 500, 68, L[i], font, cn, ch);
        }
        bc = 8;
    };
    auto buildDocMenu = [&]() 
    {
        resetUI();
        const char* L[6] = {"1. Today's Appointments",
                            "2. Mark Complete",
                            "3. Mark No-Show",       
                            "4. Write Prescription",
                            "5. Patient History",  
                            "6. Logout"};
        for (int i = 0; i < 6; ++i) 
        {
            float c = (float)(i % 2), r = (float)(i / 2);
            Color cn = (i == 5) ?Color(50, 55, 80) : doctorBar;
            Color ch = (i == 5) ?Color(70, 75, 100) :Color(79, 160, 255);
            btn[i].init(120.f + c * 560.f, 190.f + r * 88.f, 500, 68, L[i], font, cn, ch);
        }
        bc = 6;
     };
    auto buildAdmMenu = [&]() 
    {
        resetUI();
        const char* L[10] = {   "1. Add Doctor",
                                "2. Remove Doctor",
                                "3. View All Patients",
                                "4. View All Doctors",
                                "5. All Appointments",
                                "6. Unpaid Bills",
                                "7. Discharge Patient",
                                "8. Security Log",
                                "9. Daily Report",  
                                "10. Logout"};
        for (int i = 0; i < 10; ++i) 
        {
            float c = (float)(i % 2), r = (float)(i / 2);
            Color cn = (i == 9) ? Color(50, 55, 80) : adminBar;
            Color ch = (i == 9) ? Color(70, 75, 100) : Color(255, 215, 80);
            btn[i].init(120.f + c * 560.f, 155.f + r * 76.f, 500, 60, L[i], font, cn, ch);
        }
        bc = 10;
    };
    auto showMsg = [&](const char* msg, Screen ret) 
    {
        Person::strCopy(gMsg, msg, 512);
        gRetScreen = ret;
        gScreen = Screen::messageBox;
        resetUI();
        btn[0].init(490, 450, 300, 46, "OK", font);
        bc = 1;
    };
    auto goBack = [&]() 
    {
        if (gRole == Role::PATIENT) 
        {
            gScreen = Screen::patientMenu;
            buildPatMenu();
        }
        else if (gRole == Role::DOCTOR) 
        {
            gScreen = Screen::doctorMenu; 
            buildDocMenu(); 
        }
        else 
        {
            gScreen = Screen::adminMenu;
            buildAdmMenu(); 
        }
    };
    auto tryLogin = [&]()->bool
    {
        if (!Validator::isPositiveInt(inp[0].buffer))
            return false;
        int id = Validator::parseInt(inp[0].buffer);
        if (gRole == Role::PATIENT)
        {
            Patient* p = gP.findById(id);
            if (!p || !p->checkPassword(inp[1].buffer))
            {
                return false;
            }
            gUid = id; return true;
        }
        else if (gRole == Role::DOCTOR) 
        {
            Doctor* d = gD.findById(id);
            if (!d || !d->checkPassword(inp[1].buffer)) 
            {
                return false;
            }
            gUid = id; return true;
        }
        else 
        {
            if (gAdmin.getId() == id && gAdmin.checkPassword(inp[1].buffer)) 
            {
                gUid = id; 
                return true;
            }
        }
        return false;
    };
    buildRoleSelect();  // first screen
    //  Main loop 
    while (window.isOpen()) 
    {
        Vector2f mouse(Mouse::getPosition(window).x,Mouse::getPosition(window).y);
        for (int i = 0; i < bc; ++i)
        {
            btn[i].update(mouse.x, mouse.y);
        }
        Event ev;
        while (window.pollEvent(ev)) 
        {
            if (ev.type == Event::Closed) 
                window.close();

            if (ev.type == sf::Event::MouseWheelScrolled) 
            {
                gScroll -= ev.mouseWheelScroll.delta * 22.f;
                if (gScroll < 0.f)
                    gScroll = 0.f;
            }

            if (ev.type == sf::Event::TextEntered)
            {
                for (int i = 0; i < ic; ++i) 
                    inp[i].onChar(ev.text.unicode);
            }
            if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left)
            {
                Vector2f pt((float)ev.mouseButton.x, (float)ev.mouseButton.y);
                // Focus management
                for (int i = 0; i < ic; ++i)
                {
                    inp[i].focused = inp[i].rect.getGlobalBounds().contains(pt);
                }
                if (gScreen == Screen::roleSelect) 
                {
                    if (btn[0].hit(pt)) 
                    {
                        gRole = Role::PATIENT; 
                        gFails = 0; 
                        gScreen = Screen::login;
                        buildLogin();
                    }
                    else if (btn[1].hit(pt))
                    {
                        gRole = Role::DOCTOR; 
                        gFails = 0;
                        gScreen = Screen::login;
                        buildLogin();
                    }
                    else if (btn[2].hit(pt)) 
                    {
                        gRole = Role::ADMIN;
                        gFails = 0; 
                        gScreen = Screen::login;
                        buildLogin();
                    }
                    else if (btn[3].hit(pt)) 
                    {
                        window.close();
                    }
                }
                //  LOGIN
                else if (gScreen == Screen::login) 
                {
                    if (btn[1].hit(pt))
                    {
                        gRole = Role::NONE; 
                        gScreen = Screen::roleSelect;
                        buildRoleSelect();
                    }
                    else if (btn[0].hit(pt))
                    {
                        if (tryLogin())
                        {
                            gFails = 0;
                            if (gRole == Role::PATIENT) 
                            {
                                gScreen = Screen::patientMenu;
                                buildPatMenu();
                            }
                            else if (gRole == Role::DOCTOR)
                            {
                                gScreen = Screen::doctorMenu;
                                buildDocMenu();
                            }
                            else 
                            {
                                gScreen = Screen::adminMenu; 
                                buildAdmMenu();
                            }
                        }
                        else 
                        {
                            ++gFails;
                            const char* rs = (gRole == Role::PATIENT) ? "Patient" :(gRole == Role::DOCTOR) ? "Doctor" : "Admin";    // Log failed attempt
                            FileHandler::appendSecurityLog(rs, inp[0].buffer, "FAILED");
                            if (gFails >= 3)
                                showMsg("Account locked. Contact admin.", Screen::roleSelect);
                            else
                                showMsg("Invalid ID or password. Try again.", Screen::login);
                        }
                    }
                }
                //  MESSAGE BOX
                else if (gScreen == Screen::messageBox) 
                {
                    if (btn[0].hit(pt))
                    {
                        Screen r = gRetScreen;
                        if (r == Screen::roleSelect)
                        {
                            gScreen = r; 
                            buildRoleSelect();
                        }
                        else if (r == Screen::login) 
                        {
                            gScreen = r; 
                            buildLogin(); 
                        }
                        else if (r == Screen::patientMenu)
                        {
                            gScreen = r; 
                            buildPatMenu(); 
                        }
                        else if (r == Screen::doctorMenu)
                        {
                            gScreen = r; 
                            buildDocMenu();
                        }
                        else if (r == Screen::adminMenu) 
                        {
                            gScreen = r; 
                            buildAdmMenu(); 
                        }
                        else if (r == Screen::bookAppointment) 
                        {
                            gScreen = r; 
                            resetUI();
                            inp[0].init(140, 160, 380, "Specialization:", font);
                            inp[1].init(140, 270, 380, "Doctor ID:", font);
                            inp[2].init(140, 380, 380, "Date (DD-MM-YYYY):", font);
                            inp[3].init(140, 490, 380, "Time Slot (09:00-16:00):", font);
                            btn[0].init(140, 600, 170, 46, "Book", font);
                            btn[1].init(330, 600, 170, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                            bc = 2;
                            ic = 4;
                        }
                        else 
                        {
                            goBack();
                        }
                    }
                }
                //  PATIENT MENU
                else if (gScreen == Screen::patientMenu)
                {
                    if (btn[7].hit(pt))
                    {
                        gUid = -1;
                        gRole = Role::NONE;
                        gScreen = Screen::roleSelect; 
                        buildRoleSelect();
                    }
                    else if (btn[0].hit(pt))
                    {  // Book
                        gScreen = Screen::bookAppointment; 
                        resetUI();
                        inp[0].init(140, 160, 380, "Specialization:", font);
                        inp[1].init(140, 270, 380, "Doctor ID:", font);
                        inp[2].init(140, 380, 380, "Date (DD-MM-YYYY):", font);
                        inp[3].init(140, 490, 380, "Time Slot (09:00-16:00):", font);
                        btn[0].init(140, 600, 170, 46, "Book", font);
                        btn[1].init(330, 600, 170, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2;
                        ic = 4;
                    }
                    else if (btn[1].hit(pt)) 
                    {  // Cancel
                        buildMyAppointments();
                        gScreen = Screen::cancelAppointment; 
                        resetUI();
                        inp[0].init(440, 540, 380, "Appointment ID to cancel:", font);
                        btn[0].init(440, 620, 185, 46, "Cancel Appt", font, cancelButton,Color(255, 100, 100));
                        btn[1].init(645, 620, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2; 
                        ic = 1;
                    }
                    else if (btn[2].hit(pt)) 
                    {
                        buildMyAppointments(); 
                        gScreen = Screen::viewAppointment; 
                        mkBackBtn();
                    }
                    else if (btn[3].hit(pt)) 
                    {  // Medical records
                        lClear();
                        lAdd("Date       | Doctor               | Medicines\n");
                        lAdd("-----------+----------------------+-------------------------------\n");
                        bool any = false;
                        for (int i = 0; i < gRx.size(); ++i)
                        {
                            if (gRx[i].getPatientId() != gUid) continue;
                            any = true;
                            char ln[512]; 
                            ln[0] = '\0';
                            sapp(ln, gRx[i].getDate(), 512); sapp(ln, " | ", 512);
                            Doctor* d = gD.findById(gRx[i].getDoctorId());
                            sapp(ln, d ? d->getName() : "?", 512); sapp(ln, "          | ", 512);
                            sapp(ln, gRx[i].getMedicines(), 512); sapp(ln, "\n", 512);
                            lAdd(ln);
                        }
                        if (!any)
                        {
                            lAdd("No medical records found.\n");
                        }
                        gScreen = Screen::viewRecords; mkBackBtn();
                    }
                    else if (btn[4].hit(pt))
                    {
                        buildMyBills(false); 
                        gScreen = Screen::viewBill;
                        mkBackBtn();
                    }
                    else if (btn[5].hit(pt))
                    {  // Pay bill
                        buildMyBills(true);
                        gScreen = Screen::payBill;
                        resetUI();
                        inp[0].init(440, 530, 380, "Bill ID to pay:", font);
                        btn[0].init(440, 610, 185, 46, "Pay", font, dischargeButton,Color(80, 245, 180));
                        btn[1].init(645, 610, 185, 46, "< Back", font,Color(50, 55, 80), Color(70, 75, 100));
                        bc = 2;
                        ic = 1;
                    }
                    else if (btn[6].hit(pt)) 
                    {  // Top up
                        gScreen = Screen::topUp; resetUI();
                        inp[0].init(440, 300, 380, "Amount to add (PKR):", font);
                        btn[0].init(440, 380, 185, 46, "Add Funds", font, dischargeButton,Color(80, 245, 180));
                        btn[1].init(645, 380, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2;
                        ic = 1;
                    }
                }
                //  BOOK APPOINTMENT
                else if (gScreen == Screen::bookAppointment)
                {
                    if (btn[1].hit(pt))
                    {
                        gScreen = Screen::patientMenu;
                        buildPatMenu(); 
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        int did = Validator::parseInt(inp[1].buffer);
                        Doctor* doc = gD.findById(did);
                        const char* date = inp[2].buffer;
                        const char* slot = inp[3].buffer;
                        if (!doc)
                            showMsg("Doctor not found.", Screen::bookAppointment);
                        else if (!Validator::isValidDate(date))
                            showMsg("Invalid date. Use DD-MM-YYYY, current year or later.", Screen::bookAppointment);
                        else if (!Validator::isValidTimeSlot(slot))
                            showMsg("Invalid slot. Valid: 09:00 10:00 11:00 12:00 13:00 14:00 15:00 16:00", Screen::bookAppointment);
                        else
                        {
                            Appointment cand(0, gUid, did, date, slot);                     // Check conflict via overloaded ==
                            bool taken = false;
                            for (int i = 0; i < gA.size(); ++i)
                            {
                                if (gA[i] == cand)
                                {
                                    taken = true;
                                    break;
                                }
                            }
                            if (taken) 
                            {
                                try 
                                { 
                                    throw SlotUnavailableException(); 
                                }
                                catch (SlotUnavailableException& e)
                                {
                                    showMsg(e.what(), Screen::bookAppointment); 
                                }
                            }
                            else 
                            {
                                Patient* pat = gP.findById(gUid);
                                if (pat->getBalance() < doc->getFee()) 
                                {
                                    try
                                    {
                                        throw InsufficientFundsException(); 
                                    }
                                    catch (InsufficientFundsException& e) 
                                    {
                                        showMsg(e.what(), Screen::bookAppointment);
                                    }
                                }
                                else 
                                {
                                    *pat -= doc->getFee();              // overloaded -=
                                    FileHandler::updatePatient(*pat);

                                    int naid = gA.maxId() + 1;
                                    Appointment na(naid, gUid, did, date, slot);
                                    gA.add(na); 
                                    FileHandler::appendAppointment(na);

                                    time_t now = time(nullptr);
                                    struct tm* lt = localtime(&now);
                                    char tod[12];
                                    strftime(tod, 12, "%d-%m-%Y", lt);

                                    int nbid = gB.maxId() + 1;
                                    Bill nb(nbid, gUid, naid, doc->getFee(), Bill::UNPAID, tod);
                                    gB.add(nb);
                                    FileHandler::appendBill(nb);

                                    char msg[64] = "Appointment booked! ID: ";
                                    char ids[8];
                                    IntegerToCharArray(naid, ids);
                                    sapp(msg, ids, 64);
                                    showMsg(msg, Screen::patientMenu);
                                }
                            }
                        }
                    }
                }
                //  CANCEL APPOINTMENT
                else if (gScreen == Screen::cancelAppointment) 
                {
                    if (btn[1].hit(pt))
                    {
                        gScreen = Screen::patientMenu;
                        buildPatMenu();
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        int aid = Validator::parseInt(inp[0].buffer);
                        Appointment* a = nullptr;
                        for (int i = 0; i < gA.size(); ++i)
                        {
                            if (gA[i].getAppointmentId() == aid && gA[i].getPatientId() == gUid && gA[i].getStatus() == Appointment::PENDING)
                            {
                                a = &gA[i]; break;
                            }
                        }
                        if (!a)
                        {
                            showMsg("Invalid appointment ID.", Screen::patientMenu); 
                        }
                        else 
                        {
                            Doctor* d = gD.findById(a->getDoctorId());
                            float fee = d ? d->getFee() : 0.f;
                            Patient* pat = gP.findById(gUid);
                            *pat += fee;                                         // refund via overloaded +=
                            FileHandler::updatePatient(*pat);
                            a->setStatus(Appointment::CANCELLED);
                            FileHandler::updateAppointment(*a);
                            for (int i = 0; i < gB.size(); ++i)
                            {
                                if (gB[i].getAppointmentId() == aid)
                                {
                                    gB[i].setStatus(Bill::CANCELLED);
                                    FileHandler::updateBill(gB[i]);
                                    break;
                                }
                            }
                            char msg[64] = "Appointment cancelled. PKR ";
                            char fs[16]; 
                            ConvertFloatToString(fee, fs); 
                            sapp(msg, fs, 64);
                            sapp(msg, " refunded.", 64);
                            showMsg(msg, Screen::patientMenu);
                        }
                    }
                }
                //  TOP UP
                else if (gScreen == Screen::topUp)
                {
                    if (btn[1].hit(pt)) 
                    {
                        gScreen = Screen::patientMenu; 
                        buildPatMenu(); 
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        try {
                            if (!Validator::isPositiveFloat(inp[0].buffer))
                            {
                                throw InvalidInputException("Amount must be a positive number.");
                            }
                            Patient* pat = gP.findById(gUid);
                            *pat += Validator::parseFloat(inp[0].buffer);
                            FileHandler::updatePatient(*pat);
                            char msg[64] = "Balance updated. New balance: PKR ";
                            char bs[16];
                            ConvertFloatToString(pat->getBalance(), bs); 
                            sapp(msg, bs, 64);
                            showMsg(msg, Screen::patientMenu);
                        }
                        catch (InvalidInputException& e) 
                        {
                            showMsg(e.what(), Screen::topUp); 
                        }
                    }
                }
                //  PAY BILL
                else if (gScreen == Screen::payBill) 
                {
                    if (btn[1].hit(pt)) 
                    {
                        gScreen = Screen::patientMenu; 
                        buildPatMenu(); 
                    }
                    else if (btn[0].hit(pt))
                    {
                        int bid = Validator::parseInt(inp[0].buffer);
                        Bill* bl = nullptr;
                        for (int i = 0; i < gB.size(); ++i)
                        {
                            if (gB[i].getId() == bid && gB[i].getPatientId() == gUid && gB[i].getStatus() == Bill::UNPAID)
                            {
                                bl = &gB[i]; break;
                            }
                        }
                        if (!bl) 
                        { 
                            showMsg("Invalid bill ID.", Screen::patientMenu); 
                        }
                        else
                        {
                            Patient* pat = gP.findById(gUid);
                            if (pat->getBalance() < bl->getAmount()) 
                            {
                                try
                                {
                                    throw InsufficientFundsException(); 
                                }
                                catch (InsufficientFundsException& e) 
                                {
                                    showMsg(e.what(), Screen::patientMenu); 
                                }
                            }
                            else
                            {
                                *pat -= bl->getAmount();
                                FileHandler::updatePatient(*pat);
                                bl->setStatus(Bill::PAID);
                                FileHandler::updateBill(*bl);
                                char msg[64] = "Bill paid. Remaining balance: PKR ";
                                char bs[16]; 
                                ConvertFloatToString(pat->getBalance(), bs);
                                sapp(msg, bs, 64);
                                showMsg(msg, Screen::patientMenu);
                            }
                        }
                    }
                }
                //  DOCTOR MENU
                else if (gScreen == Screen::doctorMenu)
                {
                    if (btn[5].hit(pt))
                    {
                        gUid = -1; gRole = Role::NONE;
                        gScreen = Screen::roleSelect; buildRoleSelect();
                    }
                    else if (btn[0].hit(pt)) 
                    {  // Today's appointments
                        time_t now = time(nullptr);
                        struct tm* lt = localtime(&now);
                        char tod[12]; strftime(tod, 12, "%d-%m-%Y", lt);
                        lClear();
                        lAdd("ID  | Patient          | Time  | Status\n");
                        lAdd("----+------------------+-------+----------\n");
                        bool any = false;
                        for (int i = 0; i < gA.size(); ++i)
                        {
                            if (gA[i].getDoctorId() != gUid) 
                                continue;
                            if (!Person::strEqual(gA[i].getDate(), tod))
                                continue;
                            any = true;
                            char ln[128];
                            ln[0] = '\0';
                            char id[8];
                            IntegerToCharArray(gA[i].getAppointmentId(), id);
                            sapp(ln, id, 128); 
                            sapp(ln, " | ", 128);
                            Patient* p = gP.findById(gA[i].getPatientId());
                            sapp(ln, p ? p->getName() : "?", 128); 
                            sapp(ln, "       | ", 128);
                            sapp(ln, gA[i].getTimeSlot(), 128); 
                            sapp(ln, " | ", 128);
                            sapp(ln, gA[i].getStatusStr(), 128);
                            sapp(ln, "\n", 128);
                            lAdd(ln);
                        }
                        if (!any)
                        {
                            lAdd("No appointments scheduled for today.\n");
                        }
                        gScreen = Screen::doctorToday; mkBackBtn();
                    }
                    else if (btn[1].hit(pt))
                    {  // Mark complete
                        gScreen = Screen::doctorMark; resetUI();
                        inp[0].init(440, 290, 380, "Appointment ID to mark Complete:", font);
                        // Use inp[1] as a hidden mode flag: '0' = complete
                        inp[1].init(10, 760, 10, "", font);
                        inp[1].buffer[0] = '0'; inp[1].buffer[1] = '\0'; inp[1].length = 1;
                        btn[0].init(440, 370, 185, 46, "Mark Complete", font, dischargeButton,Color(80, 245, 180));
                        btn[1].init(645, 370, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2;
                        ic = 2;
                    }
                    else if (btn[2].hit(pt)) 
                    {  // Mark no-show
                        gScreen = Screen::doctorMark; resetUI();
                        inp[0].init(440, 290, 380, "Appointment ID to mark No-Show:", font);
                        inp[1].init(10, 760, 10, "", font);
                        inp[1].buffer[0] = '1'; inp[1].buffer[1] = '\0'; inp[1].length = 1;
                        btn[0].init(440, 370, 185, 46, "Mark No-Show", font, cancelButton,Color(255, 100, 100));
                        btn[1].init(645, 370, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2; ic = 2;
                    }
                    else if (btn[3].hit(pt)) 
                    {  // Write prescription
                        gScreen = Screen::doctorPrescription; resetUI();
                        inp[0].init(140, 160, 460, "Completed Appointment ID:", font);
                        inp[1].init(140, 270, 460, "Medicines (Name Dose;Name Dose;...):", font);
                        inp[2].init(140, 390, 460, "Notes (max 300 chars):", font);
                        btn[0].init(140, 490, 220, 46, "Save Prescription", font);
                        btn[1].init(380, 490, 220, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2; ic = 3;
                    }
                    else if (btn[4].hit(pt))
                    {  // Patient history
                        gScreen = Screen::doctorHistory; resetUI();
                        inp[0].init(440, 280, 380, "Patient ID:", font);
                        btn[0].init(440, 360, 185, 46, "View History", font);
                        btn[1].init(645, 360, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2; ic = 1;
                    }
                }
                else if (gScreen == Screen::doctorMark) 
                {
                    if (btn[1].hit(pt)) 
                    {
                        gScreen = Screen::doctorMenu; 
                        buildDocMenu(); 
                    }
                    else if (btn[0].hit(pt))
                    {
                        bool isNS = (inp[1].buffer[0] == '1');
                        int aid = Validator::parseInt(inp[0].buffer);
                        time_t now = time(nullptr);
                        struct tm* lt = localtime(&now);
                        char tod[12];
                        strftime(tod, 12, "%d-%m-%Y", lt);

                        Appointment* a = nullptr;
                        for (int i = 0; i < gA.size(); ++i)
                        {
                            if (gA[i].getAppointmentId() == aid && gA[i].getDoctorId() == gUid && gA[i].getStatus() == Appointment::PENDING && Person::strEqual(gA[i].getDate(), tod))
                            {
                                a = &gA[i]; break;
                            }
                        }
                        if (!a) 
                        {
                            showMsg("Invalid ID or appointment not pending today.", Screen::doctorMenu);
                        }
                        else if (isNS) 
                        {
                            a->setStatus(Appointment::NOSHOW);
                            FileHandler::updateAppointment(*a);
                            for (int i = 0; i < gB.size(); ++i)
                            {
                                if (gB[i].getAppointmentId() == aid)
                                {
                                    gB[i].setStatus(Bill::CANCELLED);
                                    FileHandler::updateBill(gB[i]);
                                    break;
                                }
                            }
                            showMsg("Appointment marked as no-show.", Screen::doctorMenu);
                        }
                        else 
                        {
                            a->setStatus(Appointment::COMPLETED);
                            FileHandler::updateAppointment(*a);
                            showMsg("Appointment marked as completed.", Screen::doctorMenu);
                        }
                    }
                }
                //  DOC: WRITE PRESCRIPTION
                else if (gScreen == Screen::doctorPrescription) 
                {
                    if (btn[1].hit(pt)) 
                    {
                        gScreen = Screen::doctorMenu; 
                        buildDocMenu(); 
                    }
                    else if (btn[0].hit(pt))
                    {
                        int aid = Validator::parseInt(inp[0].buffer);
                        Appointment* a = nullptr;
                        for (int i = 0; i < gA.size(); ++i)
                        {
                            if (gA[i].getAppointmentId() == aid && gA[i].getDoctorId() == gUid && gA[i].getStatus() == Appointment::COMPLETED)
                            {
                                a = &gA[i]; break;
                            }
                        }
                        if (!a) 
                        {
                            showMsg("Invalid ID or appointment not completed.", Screen::doctorMenu);
                        }
                        else 
                        {
                            bool exists = false;
                            for (int i = 0; i < gRx.size(); ++i)
                                if (gRx[i].getAppointmentId() == aid) 
                                {
                                    exists = true; 
                                    break;
                                }
                            if (exists)
                                showMsg("Prescription already written for this appointment.", Screen::doctorMenu);
                            else 
                            {
                                int nid = gRx.maxId() + 1;
                                Prescription np(nid, aid, a->getPatientId(), gUid,a->getDate(), inp[1].buffer, inp[2].buffer);
                                gRx.add(np); 
                                FileHandler::appendPrescription(np);
                                showMsg("Prescription saved.", Screen::doctorMenu);
                            }
                        }
                    }
                }
                //  DOC: PATIENT HISTORY
                else if (gScreen == Screen::doctorHistory) 
                {
                    if (btn[1].hit(pt))
                    {
                        gScreen = Screen::doctorMenu;
                        buildDocMenu();
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        int pid = Validator::parseInt(inp[0].buffer);
                        Patient* pat = gP.findById(pid);
                        bool access = false;
                        for (int i = 0; i < gA.size(); ++i)
                            if (gA[i].getPatientId() == pid && gA[i].getDoctorId() == gUid && gA[i].getStatus() == Appointment::COMPLETED)
                            {
                                access = true; break;
                            }
                        if (!pat || !access)
                            showMsg("Access denied. You can only view records of your own patients.", Screen::doctorMenu);
                        else 
                        {
                            lClear();
                            lAdd("Date       | Medicines\n");
                            lAdd("-----------+--------------------------------------------\n");
                            bool any = false;
                            for (int i = 0; i < gRx.size(); ++i)
                            {
                                if (gRx[i].getPatientId() != pid) 
                                    continue;
                                if (gRx[i].getDoctorId() != gUid)
                                    continue;
                                any = true;
                                char ln[512];
                                ln[0] = '\0';
                                sapp(ln, gRx[i].getDate(), 512);
                                sapp(ln, " | ", 512);
                                sapp(ln, gRx[i].getMedicines(), 512); 
                                sapp(ln, "\n", 512);
                                lAdd(ln);
                            }
                            if (!any) 
                                lAdd("No records found for this patient.\n");
                            gScreen = Screen::viewRecords; mkBackBtn();
                        }
                    }
                }
                //  ADMIN MENU
                else if (gScreen == Screen::adminMenu) 
                {
                    if (btn[9].hit(pt))
                    {
                        gUid = -1; 
                        gRole = Role::NONE;
                        gScreen = Screen::roleSelect;
                        buildRoleSelect();
                    }
                    else if (btn[0].hit(pt))
                    {  // Add doctor
                        gScreen = Screen::adminAddDoctor; resetUI();
                        inp[0].init(140, 120, 460, "Name:", font);
                        inp[1].init(140, 220, 460, "Specialization:", font);
                        inp[2].init(140, 320, 460, "Contact (11 digits):", font);
                        inp[3].init(140, 420, 460, "Password (min 6 chars):", font, true);
                        inp[4].init(140, 520, 460, "Consultation Fee (PKR):", font);
                        // Hidden mode flag '0' = add
                        inp[5].init(10, 760, 10, "", font);
                        inp[5].buffer[0] = '0'; inp[5].buffer[1] = '\0'; inp[5].length = 1;
                        btn[0].init(140, 620, 220, 46, "Add Doctor", font);
                        btn[1].init(380, 620, 220, 46, "< Back", font,Color(50, 55, 80), Color(70, 75, 100));
                        bc = 2; ic = 6;
                    }
                    else if (btn[1].hit(pt)) 
                    {  // Remove doctor
                        buildAllDoctors();
                        gScreen = Screen::adminAddDoctor; 
                        resetUI();
                        inp[0].init(440, 540, 380, "Doctor ID to remove:", font);
                        inp[5].init(10, 760, 10, "", font);
                        inp[5].buffer[0] = 'R';
                        inp[5].buffer[1] = '\0';
                        inp[5].length = 1;
                        btn[0].init(440, 620, 185, 46, "Remove", font, cancelButton,Color(255, 100, 100));
                        btn[1].init(645, 620, 185, 46, "< Back", font,Color(50, 55, 80),Color(70, 75, 100));
                        bc = 2; ic = 2; // inp[5] hidden
                    }
                    else if (btn[2].hit(pt)) 
                    {
                        buildAllPatients();  
                        gScreen = Screen::adminViewAll;
                        mkBackBtn(); 
                    }
                    else if (btn[3].hit(pt))
                    {
                        buildAllDoctors();  
                        gScreen = Screen::adminViewAll; 
                        mkBackBtn();
                    }
                    else if (btn[4].hit(pt)) 
                    {
                        buildAllAppointments();   
                        gScreen = Screen::adminViewAll;
                        mkBackBtn(); 
                    }
                    else if (btn[5].hit(pt)) 
                    {
                        buildUnpaidBills(); 
                        gScreen = Screen::adminViewAll; 
                        mkBackBtn();
                    }
                    else if (btn[6].hit(pt))
                    {  // Discharge
                        gScreen = Screen::adminDischarge;
                        resetUI();
                        inp[0].init(440, 300, 380, "Patient ID to discharge:", font);
                        btn[0].init(440, 380, 185, 46, "Discharge", font, cancelButton,Color(255, 100, 100));
                        btn[1].init(645, 380, 185, 46, "< Back", font, Color(50, 55, 80), Color(70, 75, 100));
                        bc = 2; ic = 1;
                    }
                    else if (btn[7].hit(pt)) 
                    {  // Security log
                        FileHandler::readSecurityLog(gList, 10240);
                        if (gList[0] == '\0') 
                            Person::strCopy(gList, "No security events logged.\n", 10240);
                        gScreen = Screen::adminSecurity;
                        mkBackBtn();
                    }
                    else if (btn[8].hit(pt))
                    {
                        buildDailyReport(); 
                        gScreen = Screen::adminReport;
                        mkBackBtn();
                    }
                }
                //  ADMIN: ADD / REMOVE DOCTOR
                else if (gScreen == Screen::adminAddDoctor) 
                {
                    if (btn[1].hit(pt))
                    {
                        gScreen = Screen::adminMenu;
                        buildAdmMenu();
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        bool removeMode = (inp[5].buffer[0] == 'R');
                        if (removeMode)
                        {
                            int did = Validator::parseInt(inp[0].buffer);
                            Doctor* doc = gD.findById(did);
                            if (!doc) 
                            {
                                showMsg("Doctor not found.", Screen::adminMenu);
                            }
                            else
                            {
                                bool pend = false;
                                for (int i = 0; i < gA.size(); ++i)
                                    if (gA[i].getDoctorId() == did && gA[i].getStatus() == Appointment::PENDING)
                                    {
                                        pend = true; break;
                                    }
                                if (pend)
                                    showMsg("Cannot remove: doctor has pending appointments.", Screen::adminMenu);
                                else {
                                    gD.removeById(did);
                                    FileHandler::deleteDoctor(did);
                                    showMsg("Doctor removed.", Screen::adminMenu);
                                }
                            }
                        }
                        else 
                        {
                            if (!Validator::isValidContact(inp[2].buffer))
                                showMsg("Contact must be exactly 11 numeric digits.", Screen::adminAddDoctor);
                            else if (!Validator::isValidPassword(inp[3].buffer))
                                showMsg("Password must be at least 6 characters.", Screen::adminAddDoctor);
                            else if (!Validator::isPositiveFloat(inp[4].buffer))
                                showMsg("Fee must be a positive number.", Screen::adminAddDoctor);
                            else 
                            {
                                int nid = gD.maxId() + 1;
                                Doctor nd(nid, inp[0].buffer, inp[1].buffer,inp[2].buffer, inp[3].buffer,Validator::parseFloat(inp[4].buffer));
                                gD.add(nd);
                                FileHandler::appendDoctor(nd);
                                char msg[64] = "Doctor added. ID: ";
                                char ids[8]; 
                                IntegerToCharArray(nid, ids);
                                sapp(msg, ids, 64);
                                showMsg(msg, Screen::adminMenu);
                            }
                        }
                    }
                }
                //  ADMIN: DISCHARGE
                else if (gScreen == Screen::adminDischarge) 
                {
                    if (btn[1].hit(pt)) 
                    {
                        gScreen = Screen::adminMenu;
                        buildAdmMenu(); 
                    }
                    else if (btn[0].hit(pt)) 
                    {
                        int pid = Validator::parseInt(inp[0].buffer);
                        Patient* pat = gP.findById(pid);
                        if (!pat) 
                        {
                            showMsg("Patient not found.", Screen::adminMenu); 
                        }
                        else
                        {
                            bool hasUnpaid = false, hasPend = false;
                            for (int i = 0; i < gB.size(); ++i)
                                if (gB[i].getPatientId() == pid && gB[i].getStatus() == Bill::UNPAID)
                                    hasUnpaid = true;
                            for (int i = 0; i < gA.size(); ++i)
                                if (gA[i].getPatientId() == pid && gA[i].getStatus() == Appointment::PENDING)
                                    hasPend = true;
                            if (hasUnpaid)
                                showMsg("Cannot discharge: patient has unpaid bills.", Screen::adminMenu);
                            else if (hasPend)
                                showMsg("Cannot discharge: patient has pending appointments.", Screen::adminMenu);
                            else
                            {
                                FileHandler::archivePatient(*pat, gA, gB, gRx);
                                gP.removeById(pid);
                                showMsg("Patient discharged and archived successfully.", Screen::adminMenu);
                            }
                        }
                    }
                }
                // VIEW screens 
                else if (gScreen == Screen::viewAppointment ||
                        gScreen == Screen::viewRecords ||
                        gScreen == Screen::viewBill ||
                        gScreen == Screen::doctorToday ||
                        gScreen == Screen::adminViewAll ||
                        gScreen == Screen::adminSecurity ||
                        gScreen == Screen::adminReport)
                {
                    if (bc > 0 && btn[0].hit(pt)) goBack();
                }

            }
        }
        window.clear(background);
        // Top bar
        RectangleShape topBar(sf::Vector2f(1280, 52));
        topBar.setFillColor(backgroundPanel);
        window.draw(topBar);
        drawText(window, "MediCore HMS", 24, 12, 20, patientBar);

        if (gUid >= 0) 
        {
            char info[128] = "";
            if (gRole == Role::PATIENT) 
            {
                Patient* p = gP.findById(gUid);
                if (p) 
                {
                    sapp(info, "Patient: ", 128);
                    sapp(info, p->getName(), 128);
                    sapp(info, "  |  PKR ", 128);
                    char b[16]; 
                    ConvertFloatToString(p->getBalance(), b); 
                    sapp(info, b, 128);
                }
            }
            else if (gRole == Role::DOCTOR)
            {
                Doctor* d = gD.findById(gUid);
                if (d) { sapp(info, "Dr. ", 128); 
                sapp(info, d->getName(), 128);
                }
            }
            else 
            {
                sapp(info, "Administrator", 128);
            }
            Text ut;
            ut.setFont(font);
            ut.setString(info);
            ut.setCharacterSize(13); 
            ut.setFillColor(captionButton);
            ut.setPosition(1280.f - ut.getLocalBounds().width - 24.f, 16.f);
            window.draw(ut);
        }
        switch (gScreen)
        {
        case Screen::roleSelect:
        {
            drawCard(window, 390, 175, 500, 380);
            drawCenterTitle(window, "Welcome to MediCore", 96, 40, text);
            drawCenterTitle(window, "Hospital Management System", 140, 20, captionButton);
            drawText(window, "Select your role to continue:", 450, 196, 12, captionButton);
            break;
        }
        case Screen::login:
        {
            drawCard(window, 360, 148, 560, 300);
            const char* rt = (gRole == Role::PATIENT) ? "Patient Login" : (gRole == Role::DOCTOR) ? "Doctor Login" : "Admin Login";
            Color rc = (gRole == Role::PATIENT) ? patientBar : (gRole == Role::DOCTOR) ? doctorBar : adminBar;
            drawCenterTitle(window, rt, 158, 20, rc);
            break;
        }
        case Screen::patientMenu:
        {
            Patient* p = gP.findById(gUid);
            char w[128] = "Patient Menu";
            if (p)
            {
                Person::strCopy(w, "Welcome, ", 128);
                sapp(w, p->getName(), 128);
            }
            drawCenterTitle(window, w, 78, 20, patientBar);
            drawCenterTitle(window, "What would you like to do?", 106, 12, captionButton);
            break;
        }
        case Screen::doctorMenu:
        {
            Doctor* d = gD.findById(gUid);
            char w[128] = "Doctor Menu";
            if (d)
            {
                Person::strCopy(w, "Welcome, Dr. ", 128);
                sapp(w, d->getName(), 128);
            }
            drawCenterTitle(window, w, 78, 20, doctorBar);
            if (d)
                drawCenterTitle(window, d->getSpecialization(), 106, 12, doctorBar);
            break;
        }
        case Screen::adminMenu:
        {
            drawCenterTitle(window, "Admin Panel  —  MediCore", 78, 22, adminBar);
            break;
        }
        case Screen::messageBox:
        {
            drawCard(window, 240, 258, 800, 280);
            drawText(window, gMsg, 260, 316, 15, text);
            break;
        }
        case Screen::bookAppointment:
        {
            drawCenterTitle(window, "Book an Appointment", 70, 20, patientBar);
            drawText(window, "Available slots:  09:00  10:00  11:00  12:00  13:00  14:00  15:00  16:00",
                140, 596, 11, captionButton);
            break;
        }
        case Screen::cancelAppointment:
        {
            drawCenterTitle(window, "Cancel Appointment", 70, 20, cancelButton);
            drawScrollList(window, 60, 108, 1160, 400);
            break;
        }
        case Screen::viewAppointment:
        {
            drawCenterTitle(window, "My Appointments", 70, 20, patientBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::viewRecords:
        {
            drawCenterTitle(window, "Medical Records", 70, 20, patientBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::viewBill:
        {
            drawCenterTitle(window, "My Bills", 70, 20, patientBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::payBill:
        {
            drawCenterTitle(window, "Pay a Bill", 70, 20, dischargeButton);
            drawScrollList(window, 60, 108, 1160, 400);
            break;
        }
        case Screen::topUp:
        {
            drawCard(window, 360, 218, 560, 220);
            drawCenterTitle(window, "Top Up Balance", 70, 20, patientBar);
            break;
        }
        case Screen::doctorToday:
        {
            drawCenterTitle(window, "Today's Appointments", 70, 20, doctorBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::doctorMark:
        {
            drawCenterTitle(window, (inp[1].buffer[0] == '1') ? "Mark No-Show" : "Mark Appointment Complete", 70, 20, doctorBar);
            break;
        }
        case Screen::doctorPrescription:
        {
            drawCenterTitle(window, "Write Prescription", 70, 20, doctorBar);
            break;
        }
        case Screen::doctorHistory:
        {
            drawCenterTitle(window, "Patient Medical History", 70, 20, doctorBar);
            break;
        }
        case Screen::adminAddDoctor:
        {
            drawCenterTitle(window, (inp[5].buffer[0] == 'R') ? "Remove Doctor" : "Add New Doctor", 70, 20, adminBar);
            if (inp[5].buffer[0] == 'R')
                drawScrollList(window, 60, 108, 1160, 400);
            break;
        }
        case Screen::adminViewAll:
        {
            drawCenterTitle(window, "Directory", 70, 20, adminBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::adminDischarge:
        {
            drawCenterTitle(window, "Discharge Patient", 70, 20, cancelButton);
            break;
        }
        case Screen::adminSecurity:
        {
            drawCenterTitle(window, "Security Log", 70, 20, adminBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        case Screen::adminReport:
        {
            drawCenterTitle(window, "Daily Report", 70, 20, adminBar);
            drawScrollList(window, 60, 108, 1160, 560);
            drawText(window, "Scroll with mouse wheel", 60, 682, 11, captionButton);
            break;
        }
        }
        // Draw inputs then buttons on top
        for (int i = 0; i < ic; ++i) 
            inp[i].draw(window);
        for (int i = 0; i < bc; ++i) 
            btn[i].draw(window);

        window.display();
    } // while window.isOpen()
    return 0;
}