#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "WaterButt Pi5 — v" << WATERBUTT_VERSION << std::endl;
    std::cout << "Hello from PlatformIO (native)!" << std::endl;
    for (int i = 5; i > 0; --i) {
        std::cout << "Running... " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Done." << std::endl;
    return 0;
}
