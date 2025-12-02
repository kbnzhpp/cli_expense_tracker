#include "CommandHandler.h"

#include <iostream>
#include <iterator>

int main(int argc, char* argv[]) {
    CommandHandler commandParser(argc, argv);
    try {
        commandParser.execCommand();
    } catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected error" << std::endl;
    }
    
    return 0;
}