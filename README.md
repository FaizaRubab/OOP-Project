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
*Repository github link: *
https://github.com/FaizaRubab/OOP-Project/tree/master/OOP%20Project
