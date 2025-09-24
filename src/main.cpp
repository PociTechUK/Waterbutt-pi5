// WB-REPO-SIG: Waterbutt-pi5 | file: src/main.cpp | hello-test
#include <iostream>
#include <string>

namespace wb {
    constexpr const char* kAppName = "Waterbutt Controller";
    constexpr const char* kVersion = "0.14-nuc";

    int run(int /*argc*/, char** /*argv*/) {
        std::cout << "----------------------------------------\n";
        std::cout << kAppName << " (" << kVersion << ")\n";
        std::cout << "Hello from NUC! 🚀\n";
        std::cout << "Extra NUC status line here ✅\n";
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
