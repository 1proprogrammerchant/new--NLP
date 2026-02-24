// Sample C++ integration for NLP bot
#include <iostream>
extern "C" void call_python_nlp();

int main() {
    std::cout << "C++ module: calling Python NLP core..." << std::endl;
    call_python_nlp();
    return 0;
}
