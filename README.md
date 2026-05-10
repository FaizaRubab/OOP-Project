# MediCore — Hospital Management System

> **Course:** Object Oriented Programming (OOP) &nbsp;|&nbsp; **Semester:** Spring 2026  
> **Section:** BCS-2B &nbsp;|&nbsp; **Total Marks:** 100  
> **Student:** Faiza Rubab &nbsp;|&nbsp; **Roll Number:** 25L-0660  
> **GitHub:** https://github.com/FaizaRubab/OOP-Project

---

## Table of Contents

1. [Project Overview](#project-overview)  
2. [Features](#features)  
3. [Class Structure](#class-structure)  
4. [Operator Overloads](#operator-overloads)  
5. [File Formats](#file-formats)  
6. [Project Structure](#project-structure)  
7. [How to Compile and Run](#how-to-compile-and-run)  
8. [Login Credentials (Sample Data)](#login-credentials-sample-data)  
9. [GUI Overview](#gui-overview)  
10. [Important Notes](#important-notes)

---

## Project Overview

MediCore is a fully functional **Hospital Management System** built in C++ using **SFML 2.6** for the graphical interface. It supports three user roles — **Patient**, **Doctor**, and **Admin** — and manages appointments, prescriptions, billing, and medical records.

All data is persisted in `.txt` files. Every change is immediately written to disk and fully restored on the next startup — no data is ever hardcoded.

---

## Features

### Patient
| Feature | Description |
|---|---|
| Book Appointment | Search by specialization, pick doctor, date, and time slot |
| Cancel Appointment | Cancel a pending appointment and receive a full refund |
| View Appointments | All appointments sorted by date (ascending) |
| Medical Records | All prescriptions sorted by date (most recent first) |
| View Bills | All bills with total outstanding amount |
| Pay Bill | Pay any unpaid bill directly from balance |
| Top Up Balance | Add funds to the patient account |

### Doctor
| Feature | Description |
|---|---|
| Today's Appointments | View all appointments scheduled for today |
| Mark Complete | Mark a pending appointment as completed |
| Mark No-Show | Mark a pending appointment as no-show (no refund) |
| Write Prescription | Write medicines and notes for a completed appointment |
| Patient Medical History | View prescriptions for any of the doctor's own patients |

### Admin
| Feature | Description |
|---|---|
| Add Doctor | Register a new doctor with full validation |
| Remove Doctor | Remove a doctor who has no pending appointments |
| View All Patients | Full patient list with unpaid bill counts |
| View All Doctors | Full doctor directory |
| View All Appointments | All appointments sorted by date (descending) |
| View Unpaid Bills | All unpaid bills across all patients, with `[OVERDUE]` flag |
| Discharge Patient | Archive and remove a patient (requires zero pending items) |
| Security Log | View all failed login attempts |
| Daily Report | Appointment counts, revenue, and doctor-wise summary for today |

---

## Class Structure

```
Person  (abstract base — 2 pure virtual methods)
├── Patient     (+=  -=  ==  <<)
├── Doctor      (==  <<)
└── Admin

Appointment     (==  <<)
Bill
Prescription

Storage<T>      (generic template, array-based, no std::vector)
FileHandler     (only class permitted to do file I/O)
Validator       (only class permitted to contain validation logic)

HospitalException       (base exception)
├── FileNotFoundException
├── InsufficientFundsException
├── InvalidInputException
└── SlotUnavailableException
```

---

## Operator Overloads

| Class | Operator | Purpose |
|---|---|---|
| `Appointment` | `==` | Detect scheduling conflict (same doctor + date + slot, neither cancelled) |
| `Appointment` | `<<` | Formatted console output |
| `Patient` | `+=` | Add amount to balance (top-up / refund) |
| `Patient` | `-=` | Deduct amount from balance (booking / payment) |
| `Patient` | `==` | Compare two patients by ID |
| `Patient` | `<<` | Formatted console output |
| `Doctor` | `==` | Compare two doctors by ID |
| `Doctor` | `<<` | Formatted console output |

---

## File Formats

All files use comma-separated values — one record per line, no spaces around commas.

**patients.txt**
```
patient_id,name,age,gender,contact,password,balance
1,Ahmed Ali,28,M,03001234567,pass123,15000.00
```

**doctors.txt**
```
doctor_id,name,specialization,contact,password,fee
1,Dr. Ayesha Siddiqui,Cardiology,03001112233,doc001,2500.00
```

**admin.txt**
```
admin_id,name,password
1,Admin,admin123
```

**appointments.txt**
```
appointment_id,patient_id,doctor_id,date,time_slot,status
1,1,1,15-04-2025,09:00,completed
```

**prescriptions.txt**
```
prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes
1,1,1,1,15-04-2025,Paracetamol 500mg;Amoxicillin 250mg,Take after meals
```

**bills.txt**
```
bill_id,patient_id,appointment_id,amount,status,date
1,1,1,2500.00,paid,15-04-2025
```

**security_log.txt**
```
timestamp,role,entered_id,result
15-04-2025 09:32:11,Patient,99,FAILED
```

**discharged.txt** — Same format as `patients.txt`. Archived patient records are appended here on discharge.

---

## Project Structure

```
MediCore/
│
├── include/                  # All class header files (.h)
│   ├── Person.h
│   ├── Patient.h
│   ├── Doctor.h
│   ├── Admin.h
│   ├── Appointment.h
│   ├── Bill.h
│   ├── Prescription.h
│   ├── Storage.h
│   ├── FileHandler.h
│   ├── Validator.h
│   └── HospitalException.h
│
├── src/                      # All class implementation files (.cpp)
│   ├── Person.cpp
│   ├── Patient.cpp
│   ├── Doctor.cpp
│   ├── Admin.cpp
│   ├── Appointment.cpp
│   ├── Bill.cpp
│   ├── Prescription.cpp
│   ├── FileHandler.cpp
│   ├── Validator.cpp
│   └── main.cpp
│
├── patients.txt              # Patient data
├── doctors.txt               # Doctor data
├── admin.txt                 # Admin credentials
├── appointments.txt          # Appointment records
├── prescriptions.txt         # Prescription records
├── bills.txt                 # Billing records
├── security_log.txt          # Failed login log
├── discharged.txt            # Archived discharged patients
│
├── Makefile                  # Linux / macOS build
├── SFML.props                # Visual Studio SFML property sheet
└── README.md
```

---

## How to Compile and Run

### Prerequisites

- **SFML 2.6** installed  
- **C++17** compatible compiler (GCC, Clang, or MSVC)

---

### Option 1 — Linux / macOS (Makefile)

```bash
# Install SFML on Ubuntu/Debian
sudo apt-get install libsfml-dev

# Clone and build
git clone https://github.com/FaizaRubab/OOP-Project.git
cd "OOP-Project/OOP Project"

make        # compile
make run    # compile and launch
make clean  # remove build files
```

---

### Option 2 — Windows (Visual Studio)

1. Download and extract **SFML 2.6.1** from https://www.sfml-dev.org/download.php  
2. Open the project in **Visual Studio 2022**.  
3. In **Property Manager**, right-click your configuration → *Add Existing Property Sheet* → select `SFML.props`.  
4. Open `SFML.props` and set `<SFML_DIR>` to your SFML installation path (e.g. `C:\SFML-2.6.1`).  
5. Press **Ctrl + F5** to build and run.

> The property sheet automatically copies the required SFML `.dll` files to your output directory after each build.

---

### Option 3 — Manual GCC command

```bash
g++ -std=c++17 src/*.cpp -o MediCore \
    -lsfml-graphics -lsfml-window -lsfml-system
./MediCore
```

> **Important:** Run the executable from the project root directory (where the `.txt` files live) so it can find the data files.

---

## Login Credentials (Sample Data)

### Patients
| ID | Name | Password | Balance |
|---|---|---|---|
| 1 | Ahmed Ali | pass123 | 15,000.00 |
| 2 | Sara Khan | sara456 | 8,500.00 |
| 3 | Bilal Raza | bilal789 | 3,200.00 |
| 4 | Hina Malik | hina321 | 500.00 |
| 5 | Usman Tariq | usman654 | 22,000.00 |

### Doctors
| ID | Name | Specialization | Password | Fee |
|---|---|---|---|---|
| 1 | Dr. Ayesha Siddiqui | Cardiology | doc001 | 2,500.00 |
| 2 | Dr. Kamran Mirza | Neurology | doc002 | 3,000.00 |
| 3 | Dr. Nadia Hussain | Orthopedics | doc003 | 2,000.00 |
| 4 | Dr. Faisal Qureshi | Dermatology | doc004 | 1,500.00 |
| 5 | Dr. Zara Ahmed | Pediatrics | doc005 | 1,800.00 |

### Admin
| ID | Password |
|---|---|
| 1 | admin123 |

---

## GUI Overview

The GUI is built with **SFML 2.6** and uses a warm ivory / deep-indigo design:

- **Left sidebar** — role-specific navigation menu with the logged-in user's name and balance  
- **Pill buttons** — coloured by action type (violet = patient, sky = doctor, gold = admin, rose = danger, emerald = success)  
- **Underline inputs** — Material-style text fields with an animated violet active line  
- **Scrollable list panels** — white card panels for viewing appointments, bills, reports, etc. (scroll with mouse wheel)  
- **Overlay message box** — dimmed full-screen overlay for confirmations and error messages  
- **Account lockout** — after 3 failed login attempts the session locks and the attempt is logged to `security_log.txt`

### Available Time Slots
```
09:00   10:00   11:00   12:00   13:00   14:00   15:00   16:00
```

---

## Important Notes

| Rule | Status |
|---|---|
| `std::vector` not used anywhere | ✅ |
| `std::string` not used anywhere | ✅ |
| `strcmp()`, `strtok()` not used | ✅ |
| No global variables | ✅ |
| No `goto` statements | ✅ |
| No static arrays (dynamic allocation only) | ✅ |
| No hardcoded inputs — all data from `.txt` files | ✅ |
| Each class in its own `.h` and `.cpp` file | ✅ |
| `FileHandler` is the only class doing file I/O | ✅ |
| `Validator` is the only class doing input validation | ✅ |
| Memory freed when no longer needed | ✅ |
| Manual sorting (no library sort functions) | ✅ |
| Case-insensitive specialization search using `tolower()` manually | ✅ |
| Operator overloads implemented as required | ✅ |
| Custom exceptions thrown and caught correctly | ✅ |

---

*MediCore Hospital Management System — OOP Spring 2026 — Faiza Rubab (25L-0660)*
