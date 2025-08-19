#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

struct Sale {
    string date;
    int id;
    string itemName;
    int quantity;
    double price;
};

bool isFileEmpty(const string& filename) {
    ifstream file(filename);
    return file.peek() == ifstream::traits_type::eof();
}

bool isValidDate(const string& date) {
    if (date.length() != 10 || date[2] != '/' || date[5] != '/') return false;
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));
    if (year < 1000 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    int daysInMonth[] = {31, isLeap ? 29 : 28,31,30,31,30,31,31,30,31,30,31};
    if (day < 1 || day > daysInMonth[month - 1]) return false;
    return true;
}
void inputs(string& date, string& itemName, int& quantity, double& unitPrice) {
    cout << "Enter Date (dd/mm/yyyy): ";
    cin >> date;
    while (!isValidDate(date)) {
        cout << "Invalid date format! Enter again (dd/mm/yyyy): ";
        cin >> date;
    }
    cin.ignore();
    cout << "Enter Item Name: ";
    getline(cin, itemName);
    cout << "Enter Quantity: ";
    while (!(cin >> quantity) || quantity < 0) {
        cout << "Invalid quantity! Enter again: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Enter Unit Price: ";
    while (!(cin >> unitPrice) || unitPrice < 0) {
        cout << "Invalid price! Enter again: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
}

void createRecords(const string& filename) {
    int nextId = getNextSalesID(filename);
    char choice;
    do {
        Sale s;
        s.id = nextId++;
        inputs(s.date, s.itemName, s.quantity, s.price);
        appendToCSV(filename, s);
        choice = getYesNoInput("Record saved.\nAdd another? (Y/N): ");
    } while (choice == 'y' || choice == 'Y');
}
void updateRecord(const string& filename) {
    vector<Sale> sales = loadSales(filename);
    if (sales.empty()) {
        cout << "No records to update.\n";
        return;
    }

    displayAllSales(sales);

    int id;
    cout << "Enter saleID to update: ";
    cin >> id;
    cin.ignore();
    bool found = false;

    for (auto& s : sales) {
        if (s.id == id) {
            found = true;
            cout << "Enter new date (dd/mm/yyyy): ";
            cin >> s.date;
            while (!isValidDate(s.date)) {
                cout << "Invalid date! Enter again: ";
                cin >> s.date;
            }
            cin.ignore();
            cout << "Enter new item name: ";
            getline(cin, s.itemName);
            cout << "Enter new quantity: ";
            while (!(cin >> s.quantity) || s.quantity < 0) {
                cout << "Invalid quantity! Enter again: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << "Enter new price: ";
            while (!(cin >> s.price) || s.price < 0) {
                cout << "Invalid price! Enter again: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore();
            break;
        }
    }

    if (found) {
        saveSales(filename, sales);
        cout << "Record updated.\n";
    } else {
        cout << "Record not found.\n";
    }
}
void deleteRecord(const string& filename) {
    vector<Sale> sales = loadSales(filename);
    if (sales.empty()) {
        cout << "No records to delete.\n";
        return;
    }

    displayAllSales(sales);

    int id;
    cout << "Enter saleID to delete: ";
    cin >> id;
    cin.ignore();
    auto it = remove_if(sales.begin(), sales.end(), [id](const Sale& s){ return s.id == id; });
    if (it != sales.end()) {
        sales.erase(it, sales.end());
        saveSales(filename, sales);
        cout << "Record deleted.\n";
    } else {
        cout << "Record not found.\n";
    }
}
void sortSalesByDate(const string& inputFilename, const string& outputFilename = "temp.csv") {
    vector<Sale> sales = loadSales(inputFilename);
    if (sales.empty()) return;
    sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
        return convertDMYtoISO(a.date) < convertDMYtoISO(b.date);
    });
    saveSales(outputFilename, sales);
}

void displayReport(const string& filename, const string& reportFile = "report.txt") {
    vector<Sale> sales = loadSales(filename);
    ofstream report(reportFile);
    if (!report) {
        cerr << "Cannot open report.txt for writing.\n";
        return;
    }
    if (sales.empty()) {
        report << "No sales records found.\n";
        cout << "No sales records found to generate report.\n";
        return;
    }

    map<string, vector<Sale>> grouped;
    for (const auto& s : sales) {
        grouped[convertDMYtoISO(s.date)].push_back(s);
    }

    double grandTotal = 0;
    string today = getTodayDateDMYwithDash();

    report << "c: " << today << "\n\n";
    report << "Sales Report : Stationary Items Sold\n\n";
    report << "---------------------------------------------------------------------------------------------------\n\n";
    report << "Date                SaleID             ItemName        Quantity     Price          SalesAmount\n\n";
    report << "--------------------------------------------------------------------------------------------------\n\n";

    for (const auto& [date, recs] : grouped) {
        double subtotal = 0;
        for (const auto& r : recs) {
            double salesAmount = r.quantity * r.price;
            subtotal += salesAmount;

            report << left << setw(15) << date;
            report << setw(20) << r.id;
            report << setw(25) << r.itemName;
            report << setw(13) << r.quantity;
            report << setw(15) << (int)r.price;
            report << fixed << setprecision(0) << salesAmount << "  \n";
        }
        report << "\n--------------------------------------------------------------------------------------------------------- \n\n";
        report << setw(70) << ("Subtotal for " + date + " is :") << fixed << setprecision(0) << subtotal << " \n\n";
        report << "---------------------------------------------------------------------------------------------------------- \n\n";
        grandTotal += subtotal;
    }

    report << setw(70) << "Grand Total:" << fixed << setprecision(0) << grandTotal << "\n";

    cout << "Report generated and saved to " << reportFile << "\n";
}

// ----------------- Main -----------------

int main() {
    const string salesFile = "sales.csv";
    const string tempFile = "temp.csv";

    char choice;

    // Create
    choice = getYesNoInput("Do you want to create new records? (Y/N): ");
    if (choice == 'y' || choice == 'Y') {
        createRecords(salesFile);
    }

    // Update
    choice = getYesNoInput("Do you want to update a record? (Y/N): ");
    if (choice == 'y' || choice == 'Y') {
        updateRecord(salesFile);
    }

    choice = getYesNoInput("Do you want to delete a record? (Y/N): ");
    if (choice == 'y' || choice == 'Y') {
        deleteRecord(salesFile);
    }

    // Sort and Report
    sortSalesByDate(salesFile, tempFile);
    displayReport(tempFile);

    return 0;
}

