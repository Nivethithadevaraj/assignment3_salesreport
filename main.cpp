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

