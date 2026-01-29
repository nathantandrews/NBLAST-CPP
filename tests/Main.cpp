#include "Test.hpp"
#include <iostream>

int main() {
    std::cout << "========================\n";
    std::cout << "Tests run: " << tests_run
              << ", Passed: " << tests_run - tests_failed
              << ", Failed: " << tests_failed << "\n";
    std::cout << "========================\n";
    return tests_failed == 0 ? 0 : 1;
}
