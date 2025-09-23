// WB-REPO-SIG: Waterbutt-pi5 | file: src/main.cpp | base-stub
// Minimal starting point. Replace or expand with real controller/UI logic.

#include <iostream>
#include <string>

namespace wb {
    constexpr const char* kAppName = "Waterbutt Controller";
    constexpr const char* kVersion = "0.3-stub";

    int run(int argc, char** argv) {
        std::cout << "----------------------------------------\n";
        std::cout << kAppName << " (" << kVersion << ")\n";
        std::cout << "Args:";
        for (int i = 0; i < argc; ++i) {
            std::cout << " [" << i << "]=" << (argv[i] ? argv[i] : "");
        }
        std::cout << "\nBuild OK. Replace this stub with real app logic.\n";
        std::cout << "----------------------------------------\n";
        return 0;
    }
}

int main(int argc, char** argv) {
    try {
        return wb::run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Fatal: unknown exception\n";
    }
    return 1;
}
