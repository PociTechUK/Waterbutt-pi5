#include <iostream>
#include "demo/hello.hpp"

int main() {
    std::cout << "=== Waterbutt Controller Demo v23 (clean import) ===" << std::endl;
    for (int i = 1; i <= 3; ++i) std::cout << "tick " << i << std::endl;
    hello();
    std::cout << "=== Finished v23 ===" << std::endl;
    return 0;
}