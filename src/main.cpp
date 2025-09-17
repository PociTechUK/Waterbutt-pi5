// DEST: src/main.cpp
#include <iostream>
#include "demo/hello.hpp"

int main() {
    std::cout << "=== Waterbutt Demo Main v2 (countdown) ===" << std::endl;
    for (int i = 3; i >= 1; --i) {
        std::cout << "Countdown: " << i << std::endl;
    }
    hello();
    std::cout << "=== Finished v2 ===" << std::endl;
    return 0;
}
