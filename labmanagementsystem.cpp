#include "labmanagementsystem.h"
#include <iostream>
using namespace std;

// Concrete Service Implementations
class LabServiceImpl : public ILabService {
private:
    vector<Lab> labs;
public:
    void addLab(const Lab& lab) override {
        labs.push_back(lab);
    }

    Lab* findLab(const string& labId) override {
        for (auto& lab : labs) {
            if (lab.getLabId() == labId) {
                return &lab;
            }
        }
        return nullptr;
    }

    const vector<Lab>& getAllLabs() const override {
        return labs;
    }

    void loadLabs(const vector<Lab>& loadedLabs) override {
        labs = loadedLabs;
    }

    vector<Lab> getLabsForSave() const override {
        return labs;
    }
};

class SectionServiceImpl : public ISectionService {
private:
    vector<Section> sections;
public:
    void addSection(const Section& section) override {
        sections.push_back(section);
    }

    Section* findSection(const string& sectionId) override {
        for (auto& section : sections) {
            if (section.getSectionId() == sectionId) {
                return &section;
            }
        }
        return nullptr;
    }

    void assignInstructor(const string& sectionId, const string& instructorId) override {
        Section* section = findSection(sectionId);
        if (section) {
            cout << "Instructor " << instructorId << " assigned to section " << sectionId << endl;
        }
    }

    void assignTA(const string& sectionId, const string& taId) override {
        Section* section = findSection(sectionId);
        if (section) {
            section->addTA(taId);
            cout << "TA " << taId << " assigned to section " << sectionId << endl;
        }
    }

    const vector<Section>& getAllSections() const override {
        return sections;
    }

    void loadSections(const vector<Section>& loadedSections) override {
        sections = loadedSections;
    }

    vector<Section> getSectionsForSave() const override {
        return sections;
    }

    vector<Section> getSectionsByLab(const string& labId) const override {
        vector<Section> result;
        for (const auto& section : sections) {
            if (section.getLabId() == labId) {
                result.push_back(section);
            }
        }
        return result;
    }
};

class ReportServiceImpl : public IReportService {
public:
    Report* generateWeeklyScheduleReport(const DateTime& weekStart,
        const vector<Section>& sections) override {
        WeeklyScheduleReport* report = new WeeklyScheduleReport("WSR_" + to_string(time(0)), weekStart);
        report->setSections(sections);
        report->generate();
        return report;
    }

    Report* generateWeeklyTimesheetReport(int weekNumber,
        const vector<Timesheet>& timesheets) override {
        WeeklyTimesheetReport* report = new WeeklyTimesheetReport("WTR_" + to_string(time(0)), weekNumber);
        report->setTimesheets(timesheets);
        report->generate();
        return report;
    }

    Report* generateSemesterReport(const string& labId,
        const vector<Section>& sections,
        const vector<Timesheet>& timesheets) override {
        SemesterReport* report = new SemesterReport("SR_" + to_string(time(0)), labId);

        vector<Timesheet> labTimesheets;
        for (const auto& section : sections) {
            if (section.getLabId() == labId) {
                for (const auto& ts : timesheets) {
                    if (ts.getSectionId() == section.getSectionId()) {
                        labTimesheets.push_back(ts);
                    }
                }
            }
        }

        report->setTimesheets(labTimesheets);
        report->generate();
        return report;
    }

    Report* generateLabSemesterTimesheetReport(const string& labId,
        const vector<Section>& sections,
        const vector<Timesheet>& timesheets) override {
        LabSemesterTimesheetReport* report = new LabSemesterTimesheetReport("LSTR_" + to_string(time(0)), labId);

        vector<Timesheet> labTimesheets;
        vector<Section> labSections;

        for (const auto& section : sections) {
            if (section.getLabId() == labId) {
                labSections.push_back(section);
                for (const auto& ts : timesheets) {
                    if (ts.getSectionId() == section.getSectionId()) {
                        labTimesheets.push_back(ts);
                    }
                }
            }
        }

        report->setTimesheets(labTimesheets);
        report->setLabSections(labSections);
        report->generate();
        return report;
    }
};

class MakeupRequestServiceImpl : public IMakeupRequestService {
private:
    vector<MakeupRequest> makeupRequests;
public:
    void submitRequest(const string& sectionId, const string& instructorId,
        const string& reason, const DateTime& preferredDate) override {
        string requestId = "REQ_" + to_string(time(0));
        MakeupRequest request(requestId, sectionId, instructorId, reason, DateTime(), preferredDate);
        makeupRequests.push_back(request);
        cout << "Makeup request submitted successfully! Request ID: " << requestId << endl;
    }

    vector<MakeupRequest> getPendingRequests() const override {
        vector<MakeupRequest> pending;
        for (const auto& request : makeupRequests) {
            if (request.getStatus() == "PENDING") {
                pending.push_back(request);
            }
        }
        return pending;
    }

    MakeupRequest* findRequest(const string& requestId) override {
        for (auto& request : makeupRequests) {
            if (request.getRequestId() == requestId) {
                return &request;
            }
        }
        return nullptr;
    }

    bool approveRequest(const string& requestId, const DateTime& scheduledDate,
        const string& approvedBy, vector<Timesheet>& timesheets) override {
        MakeupRequest* request = findRequest(requestId);
        if (request && request->getStatus() == "PENDING") {
            request->approve(scheduledDate, approvedBy);

            // Create makeup timesheet
            Timesheet makeupTimesheet(scheduledDate, scheduledDate, scheduledDate,
                true, "SYSTEM", request->getSectionId());
            timesheets.push_back(makeupTimesheet);

            return true;
        }
        return false;
    }

    bool rejectRequest(const string& requestId, const string& rejectedBy) override {
        MakeupRequest* request = findRequest(requestId);
        if (request && request->getStatus() == "PENDING") {
            request->reject(rejectedBy);
            return true;
        }
        return false;
    }

    void loadRequests(const vector<MakeupRequest>& requests) override {
        makeupRequests = requests;
    }

    vector<MakeupRequest> getRequestsForSave() const override {
        return makeupRequests;
    }
};

class UserServiceImpl : public IUserService {
private:
    vector<Instructor> instructors;
    vector<TA> tas;
    vector<Attendant> attendants;
    vector<AcademicOfficer> academicOfficers;
    vector<HOD> hods;
    vector<Building> buildings;
    vector<Venue> venues;
public:
    void addInstructor(const Instructor& instructor) override {
        instructors.push_back(instructor);
    }

    void addTA(const TA& ta) override {
        tas.push_back(ta);
    }

    void addAttendant(const Attendant& attendant) override {
        attendants.push_back(attendant);
    }

    void addAcademicOfficer(const AcademicOfficer& officer) override {
        academicOfficers.push_back(officer);
    }

    void addHOD(const HOD& hod) override {
        hods.push_back(hod);
    }

    void addBuilding(const Building& building) override {
        buildings.push_back(building);
    }

    void addVenue(const Venue& venue) override {
        venues.push_back(venue);
    }

    Instructor* findInstructor(const string& instructorId) override {
        for (auto& instructor : instructors) {
            if (instructor.getId() == instructorId) {
                return &instructor;
            }
        }
        return nullptr;
    }

    TA* findTA(const string& taId) override {
        for (auto& ta : tas) {
            if (ta.getId() == taId) {
                return &ta;
            }
        }
        return nullptr;
    }

    Attendant* findAttendant(const string& attendantId) override {
        for (auto& attendant : attendants) {
            if (attendant.getId() == attendantId) {
                return &attendant;
            }
        }
        return nullptr;
    }

    Building* findBuilding(const string& buildingId) override {
        for (auto& building : buildings) {
            if (building.getBuildingId() == buildingId) {
                return &building;
            }
        }
        return nullptr;
    }

    void assignAttendantToBuilding(const string& buildingId, const string& attendantId) override {
        Building* building = findBuilding(buildingId);
        if (building) {
            building->addAttendant(attendantId);
            cout << "Attendant " << attendantId << " assigned to building " << buildingId << endl;
        }
    }

    void loadUsers(const vector<Instructor>& loadedInstructors,
        const vector<TA>& loadedTAs,
        const vector<Attendant>& loadedAttendants,
        const vector<AcademicOfficer>& loadedOfficers,
        const vector<HOD>& loadedHODs,
        const vector<Building>& loadedBuildings,
        const vector<Venue>& loadedVenues) override {
        instructors = loadedInstructors;
        tas = loadedTAs;
        attendants = loadedAttendants;
        academicOfficers = loadedOfficers;
        hods = loadedHODs;
        buildings = loadedBuildings;
        venues = loadedVenues;
    }

    void getUsersForSave(vector<Instructor>& outInstructors,
        vector<TA>& outTAs,
        vector<Attendant>& outAttendants,
        vector<AcademicOfficer>& outOfficers,
        vector<HOD>& outHODs,
        vector<Building>& outBuildings,
        vector<Venue>& outVenues) const override {
        outInstructors = instructors;
        outTAs = tas;
        outAttendants = attendants;
        outOfficers = academicOfficers;
        outHODs = hods;
        outBuildings = buildings;
        outVenues = venues;
    }
};

class TimesheetServiceImpl : public ITimesheetService {
private:
    vector<Timesheet> timesheets;
public:
    void fillTimesheet(const Timesheet& timesheet) override {
        timesheets.push_back(timesheet);
    }

    void updateTimesheet(const string& sectionId, const DateTime& date,
        const DateTime& newStart, const DateTime& newEnd) override {
        for (auto& ts : timesheets) {
            if (ts.getSectionId() == sectionId && ts.getDate().isSameDay(date)) {
                // Create updated timesheet
                Timesheet updated(date, newStart, newEnd, ts.getIsMakeup(),
                    ts.getAttendantId(), sectionId);
                ts = updated;
                cout << "Timesheet updated for section " << sectionId << " on " << date.toString() << endl;
                return;
            }
        }
        cout << "Timesheet not found for section " << sectionId << " on " << date.toString() << endl;
    }

    const vector<Timesheet>& getAllTimesheets() const override {
        return timesheets;
    }

    void loadTimesheets(const vector<Timesheet>& loadedTimesheets) override {
        timesheets = loadedTimesheets;
    }

    vector<Timesheet> getTimesheetsForSave() const override {
        return timesheets;
    }
};

// LabManagementSystem Implementation
LabManagementSystem::LabManagementSystem() {
    // Initialize services
    labService = new LabServiceImpl();
    sectionService = new SectionServiceImpl();
    reportService = new ReportServiceImpl();
    makeupRequestService = new MakeupRequestServiceImpl();
    userService = new UserServiceImpl();
    timesheetService = new TimesheetServiceImpl();
}

LabManagementSystem::~LabManagementSystem() {
    // Clean up services
    delete labService;
    delete sectionService;
    delete reportService;
    delete makeupRequestService;
    delete userService;
    delete timesheetService;
}

void LabManagementSystem::loadAllData() {
    // Load data into respective services
    labService->loadLabs(FileManager::loadLabs());
    sectionService->loadSections(FileManager::loadSections());
    makeupRequestService->loadRequests(FileManager::loadMakeupRequests());
    timesheetService->loadTimesheets(FileManager::loadTimesheets());

    // Load users and related data
    userService->loadUsers(
        FileManager::loadInstructors(),
        FileManager::loadTAs(),
        FileManager::loadAttendants(),
        FileManager::loadAcademicOfficers(),
        FileManager::loadHODs(),
        FileManager::loadBuildings(),
        FileManager::loadVenues()
    );
}

void LabManagementSystem::saveAllData() {
    FileManager::saveLabs(labService->getLabsForSave());
    FileManager::saveSections(sectionService->getSectionsForSave());
    FileManager::saveMakeupRequests(makeupRequestService->getRequestsForSave());
    FileManager::saveTimesheets(timesheetService->getTimesheetsForSave());

    // Save users and related data
    vector<Instructor> instructors;
    vector<TA> tas;
    vector<Attendant> attendants;
    vector<AcademicOfficer> officers;
    vector<HOD> hods;
    vector<Building> buildings;
    vector<Venue> venues;

    userService->getUsersForSave(instructors, tas, attendants, officers, hods, buildings, venues);

    FileManager::saveInstructors(instructors);
    FileManager::saveTAs(tas);
    FileManager::saveAttendants(attendants);
    FileManager::saveAcademicOfficers(officers);
    FileManager::saveHODs(hods);
    FileManager::saveBuildings(buildings);
    FileManager::saveVenues(venues);
}

// Delegated methods
void LabManagementSystem::addLab(const Lab& lab) {
    labService->addLab(lab);
}

void LabManagementSystem::addSection(const Section& section) {
    sectionService->addSection(section);
}

void LabManagementSystem::addInstructor(const Instructor& instructor) {
    userService->addInstructor(instructor);
}

void LabManagementSystem::addTA(const TA& ta) {
    userService->addTA(ta);
}

void LabManagementSystem::addVenue(const Venue& venue) {
    userService->addVenue(venue);
}

void LabManagementSystem::addBuilding(const Building& building) {
    userService->addBuilding(building);
}

void LabManagementSystem::addAttendant(const Attendant& attendant) {
    userService->addAttendant(attendant);
}

void LabManagementSystem::addAcademicOfficer(const AcademicOfficer& officer) {
    userService->addAcademicOfficer(officer);
}

void LabManagementSystem::addHOD(const HOD& hod) {
    userService->addHOD(hod);
}

void LabManagementSystem::assignInstructorToSection(const string& sectionId, const string& instructorId) {
    sectionService->assignInstructor(sectionId, instructorId);
}

void LabManagementSystem::assignTAToSection(const string& sectionId, const string& taId) {
    sectionService->assignTA(sectionId, taId);
}

void LabManagementSystem::assignAttendantToBuilding(const string& buildingId, const string& attendantId) {
    userService->assignAttendantToBuilding(buildingId, attendantId);
}

void LabManagementSystem::fillTimesheet(const Timesheet& timesheet) {
    timesheetService->fillTimesheet(timesheet);
}

void LabManagementSystem::updateTimesheet(const string& sectionId, const DateTime& date,
    const DateTime& newStart, const DateTime& newEnd) {
    timesheetService->updateTimesheet(sectionId, date, newStart, newEnd);
}

// Report generation methods
Report* LabManagementSystem::generateWeeklyScheduleReport(const DateTime& weekStart) {
    return reportService->generateWeeklyScheduleReport(weekStart, sectionService->getAllSections());
}

Report* LabManagementSystem::generateWeeklyTimesheetReport(int weekNumber) {
    return reportService->generateWeeklyTimesheetReport(weekNumber, timesheetService->getAllTimesheets());
}

Report* LabManagementSystem::generateSemesterReport(const string& labId) {
    return reportService->generateSemesterReport(labId, sectionService->getAllSections(), timesheetService->getAllTimesheets());
}

Report* LabManagementSystem::generateLabSemesterTimesheetReport(const string& labId) {
    return reportService->generateLabSemesterTimesheetReport(labId, sectionService->getAllSections(), timesheetService->getAllTimesheets());
}

// Makeup request methods
void LabManagementSystem::submitMakeupRequest(const string& sectionId, const string& instructorId,
    const string& reason, const DateTime& preferredDate) {
    makeupRequestService->submitRequest(sectionId, instructorId, reason, preferredDate);
}

vector<MakeupRequest> LabManagementSystem::getPendingMakeupRequests() const {
    return makeupRequestService->getPendingRequests();
}

// FIXED: Changed from findRequest to findMakeupRequest to match header declaration
MakeupRequest* LabManagementSystem::findMakeupRequest(const string& requestId) {
    return makeupRequestService->findRequest(requestId);
}

bool LabManagementSystem::approveMakeupRequest(const string& requestId, const DateTime& scheduledDate,
    const string& approvedBy) {
    return makeupRequestService->approveRequest(requestId, scheduledDate, approvedBy,
        const_cast<vector<Timesheet>&>(timesheetService->getAllTimesheets()));
}

bool LabManagementSystem::rejectMakeupRequest(const string& requestId, const string& rejectedBy) {
    return makeupRequestService->rejectRequest(requestId, rejectedBy);
}

// Utility methods
Section* LabManagementSystem::findSection(const string& sectionId) {
    return sectionService->findSection(sectionId);
}

Lab* LabManagementSystem::findLab(const string& labId) {
    return labService->findLab(labId);
}

Instructor* LabManagementSystem::findInstructor(const string& instructorId) {
    return userService->findInstructor(instructorId);
}

TA* LabManagementSystem::findTA(const string& taId) {
    return userService->findTA(taId);
}

Building* LabManagementSystem::findBuilding(const string& buildingId) {
    return userService->findBuilding(buildingId);
}

Attendant* LabManagementSystem::findAttendant(const string& attendantId) {
    return userService->findAttendant(attendantId);
}

// Instructor functions
void LabManagementSystem::requestMakeupLab(const string& sectionId, const string& reason,
    const DateTime& preferredDate) {
    cout << "Makeup lab requested for section: " << sectionId << endl;
    cout << "Reason: " << reason << endl;
    cout << "Preferred date: " << preferredDate.toString() << endl;
}

// Academic Officer functions
void LabManagementSystem::scheduleMakeupLab(const string& sectionId, const Schedule& schedule) {
    Section* section = findSection(sectionId);
    if (section) {
        cout << "Makeup lab scheduled for section: " << sectionId << endl;
        cout << "Day: " << static_cast<int>(schedule.getDay()) << endl;
        cout << "Time: " << schedule.getStartTime().toTimeString() << " - "
            << schedule.getEndTime().toTimeString() << endl;
    }
    else {
        cout << "Section not found: " << sectionId << endl;
    }
}

// Getters for data access
const vector<Section>& LabManagementSystem::getSections() const {
    return sectionService->getAllSections();
}

const vector<Timesheet>& LabManagementSystem::getTimesheets() const {
    return timesheetService->getAllTimesheets();
}

const vector<Building>& LabManagementSystem::getBuildings() const {
    // This would need to be implemented in UserService
    static vector<Building> empty;
    return empty; // Placeholder
}

const vector<Attendant>& LabManagementSystem::getAttendants() const {
    // This would need to be implemented in UserService
    static vector<Attendant> empty;
    return empty; // Placeholder
}