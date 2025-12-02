#include <vector>
#include <string>

class CommandHandler;

void help(const std::vector<std::string>& args);
void addExpense(const std::vector<std::string>& args);
void updateExpense(const std::vector<std::string>& args);
void deleteExpense(const std::vector<std::string>& args);
void view(const std::vector<std::string>& args);
void summary(const std::vector<std::string>& args);
