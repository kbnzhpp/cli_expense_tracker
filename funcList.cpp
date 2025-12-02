#include "funcList.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

//use this to get information about methods
void help(const std::vector<std::string>& args) {
    std::cout << "--ALLOWED COMMANDS--:" << std::endl;
    std::cout << "add     - adds expense with description and amount" << std::endl;
    std::cout << "update  - update expense" << std::endl;
    std::cout << "delete  - delete expense" << std::endl;
    std::cout << "view    - view all expenses" << std::endl;
    std::cout << "summary - view a summary of expenses (month optional)" << std::endl;
    std::cout << "--help  - type this to see this message" << std::endl;
}

//adds an expense with defined amount and description
void addExpense(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        throw std::invalid_argument("ERROR: Usage: add --description <text> --amount <number>");
    }
    std::string desc;
    int amount;
    bool hasDesc = false;
    bool hasAmount = false;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&currentTime);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
    
    std::string date = buffer; 
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));
    
    for (int i = 0; i < args.size(); ++i) {
        if (args[i] == "--amount") {
            ++i;
            hasAmount = true;

            if ((args[i].empty()) || i >= args.size()) {
                throw std::invalid_argument("ERROR: --amount require a necessary integer argument");
            }

            try {
                amount = std::stoi(args[i]);
            } catch (const std::exception& e) {
                throw std::invalid_argument("ERROR: --amount must be a valid number");
            }

            if (amount < 0) {
                throw std::invalid_argument("ERROR: --amount argument must be positive");
            }

        } else if (args[i] == "--description") {
            hasDesc = true;
            ++i;
            if (args[i].empty() || i >= args.size()) {
                throw std::invalid_argument("ERROR: --description requiers a necessary string argument");
            }

            desc = args[i];
        } else {
            throw std::invalid_argument("ERROR: Add func only supports --description and --amount args");
        }
        
    }

    if (!hasDesc || !hasAmount) {
        throw std::invalid_argument("ERROR: \"add\" requires both of --amount and --descriptions args");
    }
    
    nlohmann::json expenses;
    std::ifstream data("data.json");
    if (!data.is_open()) {
        expenses = {
            {"expenses", nlohmann::json::array()},
            {"nextId", 1}
        };
    } else { 
        expenses = nlohmann::json::parse(data);
    }

    int current_id = expenses["nextId"].get<int>();
    nlohmann::json expense = {
        {"id", current_id},
        {"amount", amount},
        {"description", desc},
        {"year", year},
        {"month", month},
        {"day", day}
    };
    
    expenses["expenses"].push_back(expense);
    expenses["nextId"] = current_id + 1;
    data.close();

    std::ofstream out("data.json");
    out << expenses.dump(4);

    std::cout << "Successfully added expense: " << current_id << std::endl;
}

//updates existing expense
void updateExpense(const std::vector<std::string>& args) {

    if (args.size() > 2 || args[0] != "--id") {
        throw std::invalid_argument("ERROR: Too many arguments. Try update --id <id>");
    }
    std::ifstream file("data.json");

    if (!file.is_open()) {
        throw std::runtime_error("ERROR: Cannot find data file");
    }

    nlohmann::json data = nlohmann::json::parse(file);
    file.close();

    int id; 
    try {
        id = std::stoi(args[1]);
    } catch (const std::exception& e) {
        throw std::invalid_argument("ERROR: --id must be a valid number");
    }

    auto& expenses = data["expenses"];
    auto it = std::find_if(expenses.begin(), expenses.end(),
        [id](const nlohmann::json& expense) {
            return expense["id"] == id;
        });
    
    if (it == expenses.end()) {
        throw std::invalid_argument("ERROR: Entered id is not found");
    }

    bool isChanged = false;
    char flag;
    
    // Amount
    std::cout << "Change amount? Y/n" << std::endl;
    std::cin >> flag;

    if (flag == 'y' || flag == 'Y') {
        std::string newAmountStr;
        std::cout << "Enter new amount" << std::endl;
        std::cin >> newAmountStr;
        int newAmount;
        try {
            newAmount = std::stoi(newAmountStr);
        } catch (const std::exception& e) {
            throw std::invalid_argument("ERROR: Entered value must be a valid number");
        }   

        if (newAmount < 0) {
            throw std::invalid_argument("ERROR: Amount must be positive");
        }
        
        (*it)["amount"] = newAmount;
        isChanged = true;
    }

    // Description
    std::cout << "Change description? Y/n" << std::endl;
    std::cin >> flag;

    if (flag == 'y' || flag == 'Y') {
        std::string newDescription;
        std::cout << "Enter new description" << std::endl;
        std::cin >> newDescription;

        if (newDescription.empty()) {
            throw std::invalid_argument("ERROR: Description cannot be empty");
        }

        (*it)["description"] = newDescription;
        isChanged = true;
    }

    // Date
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&currentTime);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
    
    std::string date = buffer;
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));
    (*it)["year"] = year;
    (*it)["month"] = month;
    (*it)["day"] = day;

    if (isChanged) {
        std::ofstream outFile("data.json");
        outFile << data.dump(4);
        
        std::cout << "Data updated successfully!" << std::endl;
        outFile.close();    
    }
}

//deletes existing expense
void deleteExpense(const std::vector<std::string>& args) {
    if (args.size() < 2 || args[0] != "--id") {
        throw std::invalid_argument("ERROR: Expected --id. Try delete --id <your_id>");
    }

    std::ifstream file("data.json");

    if (!file.is_open()) {
        throw std::runtime_error("ERROR: Cannot find data file");
    }

    nlohmann::json data = nlohmann::json::parse(file);
    file.close();

    auto& expenses = data["expenses"];

    int id;
    try {
        id = stoi(args[1]);
    } catch (const std::exception& e) {
        throw std::runtime_error("ERROR: Received id is not a number");
    }

    auto it = std::find_if(expenses.begin(), expenses.end(), [id](const nlohmann::json& expense){ return expense["id"] == id; });
    if (it == expenses.end()) {
        throw std::runtime_error("ERROR: Expense with ID " + std::to_string(id) + " not found");
    }

    expenses.erase(it);

    std::ofstream outFile("data.json");
    outFile << data.dump(4);
    outFile.close();
}

//shows a list of all expenses held in list
void view(const std::vector<std::string>& args) {
    std::ifstream istr("data.json");
    nlohmann::json data;
    if (!istr.is_open()) {
        throw std::runtime_error("ERROR: List of expenses to view not found");
    } 
    data = nlohmann::json::parse(istr);
    istr.close();

    if (!data.contains("expenses") || !data["expenses"].is_array()) {
        throw std::runtime_error("ERROR: Invalid data format");
    }
    
    auto& expenses = data["expenses"];
    for (auto it = expenses.begin(); it != expenses.end(); ++it) {
        std::cout << "ID: " << (*it)["id"] 
        << " | Amount: $" << (*it)["amount"]
        << " | Description: " << (*it)["description"]
        << " | Date: " << (*it)["year"] << "-" << (*it)["month"] << "-" << (*it)["day"] << std::endl;
    }
}

//shows summary of values of all expenses in list
void summary(const std::vector<std::string>& args) {
    std::ifstream file("data.json");
    if (!file.is_open()) {
        throw std::runtime_error("ERROR: Cannot find data file");
    }
    const nlohmann::json data = nlohmann::json::parse(file);
    auto& expenses = data["expenses"];
    file.close();
    
    int sum = 0;
    if (!args.empty() && args[0] == "--month") {
        if (args.size() < 2) {
            throw std::invalid_argument("ERROR: --month requires a month number (1-12)");
        }

        int month;
        try {
            month = std::stoi(args[1]);
        } catch (const std::exception& e) {
            throw std::invalid_argument("ERROR: month must be a valid number");
        }

        if (month < 1 || month > 12) {
            throw std::invalid_argument("ERROR: Received month number should be 1-12");
        }
        
        for (const auto& i : expenses) {
            if (month == i["month"].get<int>()) {
                sum += i["amount"].get<int>();
            }   
        }

        std::cout << "$" << sum << std::endl;   
        return;
    }

    for (const auto& i : expenses) {
        sum += i["amount"].get<int>();
    }

    std::cout << "$" << sum << std::endl;   
}