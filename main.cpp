#include <iostream>

void runAllTests();

int main() {
    std::cout << "=== Starting Smart Pointers Lab Tests ===" << std::endl;
    
    try {
        runAllTests();
        std::cout << std::endl << "=== All tests completed successfully ===" << std::endl;
    } catch (...) {
        std::cerr << "FATAL ERROR: Test failed" << std::endl;
        return 1;
    }
    
    return 0;
}