#include <iostream>
#include <bitset>
#include "cpu.h"

int main() {
    cpu cpu;
    std::string in;
    while(false) {//cin >> in) {
        if(in.length() != 32) {
            return -1;
        }
        for (size_t i = 0; i < in.length(); i++)  {
            if (in[i] != '0' and in[i] != '1') {
                return -1;
            }
        }
    }

    //blockset s;
    //block* b = new block;
    //b->valid = 1;
    //std::cout << s.root->next->next->set;
    /*
    std::bitset<4> b;
    if (b == 0) {
    std::cout << 'y'; }
    */
    std::cout << std::endl;
    return 0;
}
