#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <stack>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <ctime>
using namespace std;

// PATIENT NODE - SINGLY LINKED LIST
struct Patient {
    int token;
    string name;
    int age;
    string gender;
    string phone;
    string doctor;
    string department;
    string priority;
    string status;
    Patient* next;

    Patient() {
        next = nullptr;
    }
};

// VISIT NODE - LINKED LIST
struct Visit {
    int token;
    string name;
    string diagnosis;
    string prescription;
    Visit* next;

    Visit(int t, string n, string d, string p) {
        token = t;
        name = n;
        diagnosis = d;
        prescription = p;
        next = nullptr;
    }
};

// PRIORITY QUEUE ITEM
struct QueueItem {
    int token;
    int priority;
    int order;

    bool operator<(const QueueItem& other) const {
        if (priority != other.priority)
            return priority < other.priority;

        return order > other.order;
    }
};

class Hospital {
private:
    Patient* head;
    Visit* visitHead;

    priority_queue<QueueItem> opdQueue;
    queue<int> normalQueue;
    stack<string> activityStack;

    map<string, string> doctors;

    int nextToken;
    int arrivalOrder;

    string getTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string result = dt ? dt : "Time unavailable";
        if (!result.empty() && result.back() == '\n')
            result.pop_back();
        return result;
    }

    int getPriority(string p) {
        if (p == "Emergency") return 3;
        if (p == "Urgent") return 2;
        return 1;
    }

    Patient* searchByToken(int token) {
        Patient* temp = head;

        while (temp != nullptr) {
            if (temp->token == token)
                return temp;

            temp = temp->next;
        }

        return nullptr;
    }

    void saveData() {
        ofstream file("patients.txt");

        Patient* temp = head;

        while (temp != nullptr) {
            file << temp->token << "|"
                 << temp->name << "|"
                 << temp->age << "|"
                 << temp->gender << "|"
                 << temp->phone << "|"
                 << temp->doctor << "|"
                 << temp->department << "|"
                 << temp->priority << "|"
                 << temp->status << "\n";

            temp = temp->next;
        }

        file.close();
    }

    void loadData() {
        ifstream file("patients.txt");
        if (!file) return;

        string line;

        while (getline(file, line)) {
            vector<string> data;
            string field = "";

            for (char ch : line) {
                if (ch == '|') {
                    data.push_back(field);
                    field = "";
                } else {
                    field += ch;
                }
            }
            data.push_back(field);

            if (data.size() != 9) continue;

            Patient* p = new Patient();

            try {
                p->token = stoi(data[0]);
                p->age = stoi(data[2]);
            } catch (...) {
                delete p;
                continue;
            }

            p->name = data[1];
            p->gender = data[3];
            p->phone = data[4];
            p->doctor = data[5];
            p->department = data[6];
            p->priority = data[7];
            p->status = data[8];

            p->next = head;
            head = p;

            nextToken = max(nextToken, p->token + 1);

            if (p->status == "Waiting") {
                opdQueue.push({
                    p->token,
                    getPriority(p->priority),
                    arrivalOrder++
                });

                if (p->priority == "Normal")
                    normalQueue.push(p->token);
            }
        }

        file.close();
    }

    void addVisit(int token, string name,
                  string diagnosis, string prescription) {

        Visit* v = new Visit(
            token, name, diagnosis, prescription
        );

        v->next = visitHead;
        visitHead = v;
    }

public:
    Hospital() {
        head = nullptr;
        visitHead = nullptr;
        nextToken = 1001;
        arrivalOrder = 0;

        doctors["Dr. Sharma"] = "General Medicine";
        doctors["Dr. Neha"] = "Cardiology";
        doctors["Dr. Khan"] = "Orthopedics";
        doctors["Dr. Joshi"] = "Pediatrics";
        doctors["Dr. Mehta"] = "Dermatology";

        loadData();
    }

    ~Hospital() {
        while (head != nullptr) {
            Patient* temp = head;
            head = head->next;
            delete temp;
        }

        while (visitHead != nullptr) {
            Visit* temp = visitHead;
            visitHead = visitHead->next;
            delete temp;
        }
    }

    // 1. REGISTER PATIENT - LINKED LIST
    void registerPatient() {
        Patient* p = new Patient();

        p->token = nextToken++;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "\nEnter patient name: ";
        getline(cin, p->name);

        if (p->name.empty()) {
            cout << "Name cannot be empty.\n";
            delete p;
            return;
        }

        cout << "Enter age: ";
        cin >> p->age;

        if (cin.fail() || p->age < 1 || p->age > 120) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid age.\n";
            delete p;
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter gender: ";
        getline(cin, p->gender);

        cout << "Enter phone: ";
        getline(cin, p->phone);

        cout << "\nAvailable Doctors:\n";

        int i = 1;
        vector<pair<string, string>> list(
            doctors.begin(), doctors.end()
        );

        for (auto d : list) {
            cout << i++ << ". " << d.first
                 << " - " << d.second << "\n";
        }

        int choice;
        cout << "Select doctor: ";
        cin >> choice;

        if (choice < 1 || choice > (int)list.size()) {
            cout << "Invalid doctor selection.\n";
            delete p;
            return;
        }

        p->doctor = list[choice - 1].first;
        p->department = list[choice - 1].second;

        cout << "\n1. Normal\n2. Urgent\n3. Emergency\n";
        int priorityChoice;

        cout << "Select priority: ";
        cin >> priorityChoice;

        if (priorityChoice < 1 || priorityChoice > 3) {
            cout << "Invalid priority.\n";
            delete p;
            return;
        }

        if (priorityChoice == 3)
            p->priority = "Emergency";
        else if (priorityChoice == 2)
            p->priority = "Urgent";
        else
            p->priority = "Normal";

        p->status = "Waiting";

        // Insert at beginning of linked list
        p->next = head;
        head = p;

        // Insert into priority queue
        opdQueue.push({
            p->token,
            getPriority(p->priority),
            arrivalOrder++
        });

        // Normal FIFO queue
        if (p->priority == "Normal")
            normalQueue.push(p->token);

        activityStack.push(
            "Registered patient: " + p->name
        );

        saveData();

        cout << "\nPatient registered successfully!\n";
        cout << "Token Number: " << p->token << "\n";
        cout << "Doctor: " << p->doctor << "\n";
        cout << "Priority: " << p->priority << "\n";
    }

    // 2. DISPLAY ALL PATIENTS
    void displayPatients() {
        cout << "\n========== PATIENT RECORDS ==========\n";

        if (head == nullptr) {
            cout << "No patients registered.\n";
            return;
        }

        Patient* temp = head;

        while (temp != nullptr) {
            cout << "\nToken: " << temp->token;
            cout << "\nName: " << temp->name;
            cout << "\nAge: " << temp->age;
            cout << "\nDoctor: " << temp->doctor;
            cout << "\nDepartment: " << temp->department;
            cout << "\nPriority: " << temp->priority;
            cout << "\nStatus: " << temp->status << "\n";

            temp = temp->next;
        }
    }

    // 3. SEARCH PATIENT - LINEAR SEARCH
    void searchPatient() {
        int token;

        cout << "\nEnter patient token: ";
        cin >> token;

        Patient* result = searchByToken(token);

        if (result == nullptr) {
            cout << "Patient not found.\n";
            return;
        }

        cout << "\nPatient Found!\n";
        cout << "Name: " << result->name << "\n";
        cout << "Token: " << result->token << "\n";
        cout << "Doctor: " << result->doctor << "\n";
        cout << "Status: " << result->status << "\n";
    }

    // 4. DISPLAY PRIORITY QUEUE
    void displayQueue() {
        priority_queue<QueueItem> temp = opdQueue;

        cout << "\n========== OPD PRIORITY QUEUE ==========\n";

        bool found = false;

        while (!temp.empty()) {
            QueueItem item = temp.top();
            temp.pop();

            Patient* p = searchByToken(item.token);

            if (p == nullptr || p->status != "Waiting")
                continue;

            found = true;

            cout << "Token: " << p->token
            
                 << " | Name: " << p->name
                 << " | Priority: " << p->priority
                 << "\n";
        }

        if (!found)
            cout << "No patients waiting.\n";
    }

    // 5. CALL NEXT PATIENT
    void callNextPatient() {
        while (!opdQueue.empty()) {
            QueueItem item = opdQueue.top();
            opdQueue.pop();

            Patient* p = searchByToken(item.token);

            if (p == nullptr || p->status != "Waiting")
                continue;

            p->status = "In Consultation";

            cout << "\nCalling Patient!\n";
            cout << "Token: " << p->token << "\n";
            cout << "Name: " << p->name << "\n";
            cout << "Priority: " << p->priority << "\n";
            cout << "Doctor: " << p->doctor << "\n";

            activityStack.push(
                "Called patient: " + p->name
            );

            saveData();
            return;
        }

        cout << "No patients in waiting queue.\n";
    }

    // 6. COMPLETE CONSULTATION
    void completeConsultation() {
        int token;

        cout << "\nEnter patient token: ";
        cin >> token;

        Patient* p = searchByToken(token);

        if (p == nullptr) {
            cout << "Patient not found.\n";
            return;
        }

        if (p->status != "In Consultation") {
            cout << "Patient is not currently in consultation.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string diagnosis, prescription;

        cout << "Enter diagnosis: ";
        getline(cin, diagnosis);

        cout << "Enter prescription: ";
        getline(cin, prescription);

        p->status = "Completed";

        addVisit(
            p->token,
            p->name,
            diagnosis,
            prescription
        );

        activityStack.push(
            "Completed consultation: " + p->name
        );

        saveData();

        cout << "Consultation completed successfully!\n";
    }

    // 7. VISIT HISTORY - LINKED LIST
    void visitHistory() {
        cout << "\n========== VISIT HISTORY ==========\n";

        if (visitHead == nullptr) {
            cout << "No completed visits.\n";
            return;
        }

        Visit* temp = visitHead;

        while (temp != nullptr) {
            cout << "\nToken: " << temp->token;
            cout << "\nPatient: " << temp->name;
            cout << "\nDiagnosis: " << temp->diagnosis;
            cout << "\nPrescription: " << temp->prescription;
            cout << "\n";

            temp = temp->next;
        }
    }

    // 8. DOCTOR ROSTER - MAP
    void showDoctors() {
        cout << "\n========== DOCTOR ROSTER ==========\n";

        for (auto d : doctors) {
            cout << d.first << " - "
                 << d.second << "\n";
        }
    }

    // 9. ADD DOCTOR
    void addDoctor() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string name, department;

        cout << "Enter doctor name: ";
        getline(cin, name);

        cout << "Enter department: ";
        getline(cin, department);

        if (name.empty() || department.empty()) {
            cout << "Both fields are required.\n";
            return;
        }

        if (doctors.count(name)) {
            cout << "Doctor already exists.\n";
            return;
        }

        doctors[name] = department;

        activityStack.push("Added doctor: " + name);

        cout << "Doctor added successfully.\n";
    }

    // 10. SORT PATIENTS BY NAME
    void sortPatients() {
        vector<Patient*> list;

        Patient* temp = head;

        while (temp != nullptr) {
            list.push_back(temp);
            temp = temp->next;
        }

        sort(
            list.begin(),
            list.end(),
            [](Patient* a, Patient* b) {
                return a->name < b->name;
            }
        );

        cout << "\n========== PATIENTS SORTED BY NAME ==========\n";

        for (Patient* p : list) {
            cout << p->name
                 << " | Token: " << p->token
                 << " | Status: " << p->status
                 << "\n";
        }
    }

    // 11. STACK - RECENT ACTIVITY
    void showActivities() {
        cout << "\n========== RECENT ACTIVITIES (STACK) ==========\n";

        if (activityStack.empty()) {
            cout << "No activities in this session.\n";
            return;
        }

        stack<string> temp = activityStack;

        while (!temp.empty()) {
            cout << temp.top() << "\n";
            temp.pop();
        }
    }

    // 12. DASHBOARD
    void dashboard() {
        int total = 0;
        int waiting = 0;
        int consulting = 0;
        int completed = 0;

        Patient* temp = head;

        while (temp != nullptr) {
            total++;

            if (temp->status == "Waiting")
                waiting++;
            else if (temp->status == "In Consultation")
                consulting++;
            else if (temp->status == "Completed")
                completed++;

            temp = temp->next;
        }

        cout << "\n========== HOSPITAL DASHBOARD ==========\n";
        cout << "Total Patients: " << total << "\n";
        cout << "Waiting: " << waiting << "\n";
        cout << "In Consultation: " << consulting << "\n";
        cout << "Completed: " << completed << "\n";
        cout << "Doctors: " << doctors.size() << "\n";
    }

    void menu() {
        int choice;

        do {
            cout << "\n\n";
            cout << "=========================================\n";
            cout << "       MEDICORE HOSPITAL SYSTEM\n";
            cout << "          C++ AND DSA PROJECT\n";
            cout << "=========================================\n";

            cout << "1. Dashboard\n";
            cout << "2. Register Patient\n";
            cout << "3. Display All Patients\n";
            cout << "4. Search Patient\n";
            cout << "5. Display OPD Queue\n";
            cout << "6. Call Next Patient\n";
            cout << "7. Complete Consultation\n";
            cout << "8. View Visit History\n";
            cout << "9. View Doctor Roster\n";
            cout << "10. Add Doctor\n";
            cout << "11. Sort Patients by Name\n";
            cout << "12. Recent Activities\n";
            cout << "0. Save and Exit\n";

            cout << "\nEnter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a valid number.\n";
                continue;
            }

            switch (choice) {
                case 1: dashboard(); break;
                case 2: registerPatient(); break;
                case 3: displayPatients(); break;
                case 4: searchPatient(); break;
                case 5: displayQueue(); break;
                case 6: callNextPatient(); break;
                case 7: completeConsultation(); break;
                case 8: visitHistory(); break;
                case 9: showDoctors(); break;
                case 10: addDoctor(); break;
                case 11: sortPatients(); break;
                case 12: showActivities(); break;

                case 0:
                    saveData();
                    cout << "Data saved. Thank you!\n";
                    break;

                default:
                    cout << "Invalid choice. Try again.\n";
            }

        } while (choice != 0);
    }
};

int main() {
    Hospital hospital;
    hospital.menu();
    return 0;
}