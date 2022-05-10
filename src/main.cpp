#include "SetCalculator.h"
#include <sstream>
#include <iostream>

int main()
try {
    std::istringstream temp;
    SetCalculator(std::cin, std::cout, temp).run();
}
catch (...) {
    std::cerr << "Unknown exception\n"; return EXIT_FAILURE;
}
