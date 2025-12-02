#include "CommandHandler.h"

#include <iostream>

CommandHandler::CommandHandler(const int argc, char** argv) 
    : programName_(argv[0]) 
{
    for (int i = 0; i < argc; ++i) {
        argv_.push_back((argv[i]));
    }   
}

void CommandHandler::execCommand() {
    if (argv_.size() < 2) {
        commands_["view"]({});
        return;
    }

    auto it = commands_.find(argv_[1]);
    if (it != commands_.end()) {
        std::vector<std::string> args;
        if (argv_.size() > 2) {
            args.assign(argv_.begin() + 2, argv_.end());
        }
        it->second(args);
    } else {
        throw std::invalid_argument("ERROR: Recieved command is not found. Try -h or --help");
    }
}