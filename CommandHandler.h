#include "funcList.h"

#include <vector>
#include <string>
#include <functional>

using func = std::function<void(const std::vector<std::string>&)>;

class CommandHandler {
public:
    CommandHandler() = delete;
    CommandHandler(const int argc, char** argv);
    ~CommandHandler() = default;
    
    void execCommand();

private:
    std::vector<std::string> argv_;
    std::string programName_{ "noName" };
    func currentCommand_{ help };
    std::unordered_map<std::string, func> commands_ = {
        {"--help", help},
        {"-h", help},
        {"add", addExpense},
        {"update", updateExpense},
        {"delete", deleteExpense},
        {"view", view},
        {"summary", summary},
    };
};