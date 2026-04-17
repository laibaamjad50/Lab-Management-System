#pragma once

#include "lab.h"
#include "section.h"
#include "instructor.h"
#include "ta.h"
#include "venue.h"
#include "timesheet.h"
#include "report.h"
#include "building.h"
#include "attendant.h"
#include "academicofficer.h"
#include "hod.h"
#include "filemanager.h"
#include "MakeupRequest.h"
#include <vector>
#include <iostream>
using namespace std;

// Service Interfaces
class ILabService {
public:
    virtual ~ILabService() = default;
    virtual void addLab(const Lab& lab) = 0;
    virtual Lab* findLab(const string& labId) = 0;
    virtual const vector<Lab>& getAllLabs() const = 0;
    virtual void loadLabs(const vector<Lab>& labs) = 0;
    virtual vector<Lab> getLabsForSave() const = 0;
};

class ISectionService {
public:
    virtual ~ISectionService() = default;
    virtual void addSection(const Section& section) = 0;
    virtual Section* findSection(const string& sectionId) = 0;
    virtual void assignInstructor(const string& sectionId, const string& instructorId) = 0;
    virtual void assignTA(const string& sectionId, const string& taId) = 0;
    virtual const vector<Section>& getAllSections() const = 0;
    virtual void loadSections(const vector<Section>& sections) = 0;
    virtual vector<Section> getSectionsForSave() const = 0;
    virtual vector<Section> getSectionsByLab(const string& labId) const = 0;
};

class IReportService {
public:
    virtual ~IReportService() = default;
    virtual Report* generateWeeklyScheduleReport(const DateTime& weekStart,
        const vector<Section>& sections) = 0;
    virtual Report* generateWeeklyTimesheetReport(int weekNumber,
        const vector<Timesheet>& timesheets) = 0;
    virtual Report* generateSemesterReport(const string& labId,
        const vector<Section>& sections,
        const vector<Timesheet>& timesheets) = 0;
    virtual Report* generateLabSemesterTimesheetReport(const string& labId,
        const vector<Section>& sections,
        const vector<Timesheet>& timesheets) = 0;
};

class IMakeupRequestService {
public:
    virtual ~IMakeupRequestService() = default;
    virtual void submitRequest(const string& sectionId, const string& instructorId,
        const string& reason, const DateTime& preferredDate) = 0;
    virtual vector<MakeupRequest> getPendingRequests() const = 0;
    virtual MakeupRequest* findRequest(const string& requestId) = 0;
    virtual bool approveRequest(const string& requestId, const DateTime& scheduledDate,
        const string& approvedBy, vector<Timesheet>& timesheets) = 0;
    virtual bool rejectRequest(const string& requestId, const string& rejectedBy) = 0;
    virtual void loadRequests(const vector<MakeupRequest>& requests) = 0;
    virtual vector<MakeupRequest> getRequestsForSave() const = 0;
};

class IUserService {
public:
    virtual ~IUserService() = default;
    virtual void addInstructor(const Instructor& instructor) = 0;
    virtual void addTA(const TA& ta) = 0;
    virtual void addAttendant(const Attendant& attendant) = 0;
    virtual void addAcademicOfficer(const AcademicOfficer& officer) = 0;
    virtual void addHOD(const HOD& hod) = 0;
    virtual Instructor* findInstructor(const string& instructorId) = 0;
    virtual TA* findTA(const string& taId) = 0;
    virtual Attendant* findAttendant(const string& attendantId) = 0;
    virtual Building* findBuilding(const string& buildingId) = 0;
    virtual void addBuilding(const Building& building) = 0;
    virtual void addVenue(const Venue& venue) = 0;
    virtual void assignAttendantToBuilding(const string& buildingId, const string& attendantId) = 0;
    virtual void loadUsers(const vector<Instructor>& instructors,
        const vector<TA>& tas,
        const vector<Attendant>& attendants,
        const vector<AcademicOfficer>& officers,
        const vector<HOD>& hods,
        const vector<Building>& buildings,
        const vector<Venue>& venues) = 0;
    virtual void getUsersForSave(vector<Instructor>& instructors,
        vector<TA>& tas,
        vector<Attendant>& attendants,
        vector<AcademicOfficer>& officers,
        vector<HOD>& hods,
        vector<Building>& buildings,
        vector<Venue>& venues) const = 0;
};

class ITimesheetService {
public:
    virtual ~ITimesheetService() = default;
    virtual void fillTimesheet(const Timesheet& timesheet) = 0;
    virtual void updateTimesheet(const string& sectionId, const DateTime& date,
        const DateTime& newStart, const DateTime& newEnd) = 0;
    virtual const vector<Timesheet>& getAllTimesheets() const = 0;
    virtual void loadTimesheets(const vector<Timesheet>& timesheets) = 0;
    virtual vector<Timesheet> getTimesheetsForSave() const = 0;
};

class LabManagementSystem {
private:
    // Service instances
    ILabService* labService;
    ISectionService* sectionService;
    IReportService* reportService;
    IMakeupRequestService* makeupRequestService;
    IUserService* userService;
    ITimesheetService* timesheetService;

    // Data that needs to be shared across services
    vector<Venue> venues;
    vector<Timesheet> timesheets;
    vector<Building> buildings;

public:
    LabManagementSystem();
    ~LabManagementSystem();

    // Data management methods
    void loadAllData();
    void saveAllData();

    // Academic Officer functions
    void addLab(const Lab& lab);
    void addSection(const Section& section);
    void addInstructor(const Instructor& instructor);
    void addTA(const TA& ta);
    void addVenue(const Venue& venue);
    void addBuilding(const Building& building);
    void addAttendant(const Attendant& attendant);
    void addAcademicOfficer(const AcademicOfficer& officer);
    void addHOD(const HOD& hod);

    // Assignment functions
    void assignInstructorToSection(const string& sectionId, const string& instructorId);
    void assignTAToSection(const string& sectionId, const string& taId);
    void assignAttendantToBuilding(const string& buildingId, const string& attendantId);

    // Attendant functions
    void fillTimesheet(const Timesheet& timesheet);
    void updateTimesheet(const string& sectionId, const DateTime& date,
        const DateTime& newStart, const DateTime& newEnd);

    // Instructor functions
    void requestMakeupLab(const string& sectionId, const string& reason,
        const DateTime& preferredDate);

    // Academic Officer functions
    void scheduleMakeupLab(const string& sectionId, const Schedule& schedule);

    // HOD functions
    Report* generateWeeklyScheduleReport(const DateTime& weekStart);
    Report* generateWeeklyTimesheetReport(int weekNumber);
    Report* generateSemesterReport(const string& labId);
    Report* generateLabSemesterTimesheetReport(const string& labId);

    // Makeup Request functions
    void submitMakeupRequest(const string& sectionId, const string& instructorId,
        const string& reason, const DateTime& preferredDate);

    vector<MakeupRequest> getPendingMakeupRequests() const;
    MakeupRequest* findMakeupRequest(const string& requestId);  // Changed from findRequest to findMakeupRequest
    bool approveMakeupRequest(const string& requestId, const DateTime& scheduledDate,
        const string& approvedBy);
    bool rejectMakeupRequest(const string& requestId, const string& rejectedBy);

    // Utility methods
    Section* findSection(const string& sectionId);
    Lab* findLab(const string& labId);
    Instructor* findInstructor(const string& instructorId);
    TA* findTA(const string& taId);
    Building* findBuilding(const string& buildingId);
    Attendant* findAttendant(const string& attendantId);

    // Getters for data access
    const vector<Section>& getSections() const;
    const vector<Timesheet>& getTimesheets() const;
    const vector<Building>& getBuildings() const;
    const vector<Attendant>& getAttendants() const;
};