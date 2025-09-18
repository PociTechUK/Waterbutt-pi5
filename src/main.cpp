#include <iostream>
#include "demo/hello.hpp"

int main() {
    std::cout << "=== Waterbutt Controller Demo v25 (patch) ===" << std::endl;
    std::cout << "mode: squares" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        std::cout << (i * i) << std::endl;
    }
    hello();
    std::cout << "=== Finished v25 ===" << std::endl;
    return 0;
}