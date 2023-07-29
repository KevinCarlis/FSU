#pragma once
#include <string>

const unsigned short BLOCKSIZE = 2;
const unsigned short SETSIZE = 8;
const unsigned short REGSIZE = 8;
const unsigned short MEMSIZE = 128;

unsigned short bstous(std::string bstr);
std::string ustobs(unsigned short us, size_t bits);
std::string itobs(int bint);

struct block {
    bool valid = false;
    bool wb = false;
    unsigned short tag = 0;
    int data = 0;
};

struct instruction_register {
    unsigned short operation;
    unsigned short rs;
    unsigned short rt;
    unsigned short offset;
};

class cpu {
    instruction_register ir;
    int registers[REGSIZE + 1] = {0}; //[0]=$zero; [1]=$s0
    int memory[MEMSIZE];
    block cache[SETSIZE][BLOCKSIZE];
    bool lru[SETSIZE] = {0};
    unsigned int mem_addr;
    unsigned short index;
    unsigned short tag;
    void decode(std::string instruction);
    void compute_address();
    block* determine_set();
public:
    cpu();
    void run(std::string instruction);
    void print_registers();
    void print_cache();
    void print_memory();
};
