#include <iostream>
#include "demo/hello.hpp"

int main() {
    std::cout << "=== Waterbutt Controller Demo v24 (patch) ===" << std::endl;
    for (int i = 3; i >= 1; --i) std::cout << "count " << i << std::endl;
    hello();
    std::cout << "=== Finished v24 ===" << std::endl;
    return 0;
}