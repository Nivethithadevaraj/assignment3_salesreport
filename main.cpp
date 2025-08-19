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

