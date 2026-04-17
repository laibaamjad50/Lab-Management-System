# Lab Management System

A comprehensive C++ university laboratory management system designed to streamline lab operations, scheduling, attendance tracking, and reporting.

## Features

### Role-Based Access Control
- **Academic Officers**: Manage labs, sections, venues, and user assignments
- **Instructors**: Request makeup labs and view schedules
- **Attendants**: Fill and update timesheets for lab sessions
- **HODs**: Generate various analytical reports
- **System Administrators**: Manage user accounts and roles

### Core Functionality
- **Lab & Section Management**: Create and organize labs with multiple sections
- **Schedule Management**: Schedule labs with day/time validation (no weekends, future dates only)
- **Timesheet Tracking**: Record actual lab session hours with comparison to scheduled hours
- **Makeup Request System**: Submit, approve, and schedule makeup lab sessions
- **Attendance Monitoring**: Track leaves, contact hours, and attendance rates

### Reporting System
- Weekly Schedule Reports
- Weekly Timesheet Reports  
- Semester Performance Reports
- Lab Semester Timesheet Reports with detailed statistics

### Data Persistence
- Binary file storage for all entities
- Automatic save/load functionality
- Separate data files for each entity type

## Technical Details

- **Language**: C++17
- **Storage**: Binary file serialization
- **Architecture**: Service-based design with clear separation of concerns
- **Validation**: Comprehensive date/time validation with future date constraints

## Entity Types

- Labs & Sections
- Instructors, TAs, Attendants
- Academic Officers & HODs
- Venues & Buildings
- Timesheets & Schedules
- Makeup Requests

## Getting Started

### Prerequisites
- C++17 compatible compiler
- Visual Studio 2019/2022 (Windows) or GCC (Linux)

### Building the Project
```bash
# Compile all source files
g++ *.cpp -o LabManagementSystem

# Run the executable
./LabManagementSystem
