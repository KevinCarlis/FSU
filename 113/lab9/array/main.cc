#include <iostream>
#include "cpu.h"

int main() {
    cpu cpu;
    std::string in;
    while(std::cin >> in) {
        std::cout << in << '\t';
        cpu.run(in);
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
    cpu.print_registers();
    std::cout << std::endl << std::endl;
    cpu.print_cache();
    std::cout << std::endl << std::endl;
    cpu.print_memory();
    return 0;
}
