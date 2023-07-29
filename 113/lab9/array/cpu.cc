#include <cmath>
#include <iostream>
#include "cpu.h"

unsigned short bstous(std::string bstr) {
    if(bstr.length() > 16) {
        throw std::invalid_argument("Binary string larger than 2 bytes");
    }
    unsigned short ret = 0;
    size_t i = bstr.length();
    for (size_t power = 0; power < bstr.length(); power++) {
        i = i - 1;
        if(bstr[i] == '1') {
            ret += std::pow(2, (power));
        }
        else if(bstr[i] != '0') {
            throw std::invalid_argument(
                    "Binary string contains invalid char: " + bstr[i]
            );
        }
    }
    return ret;
}

std::string ustobs(unsigned short us, size_t bits) {
    std::string ret = "";
    for(size_t i = 0; i < bits; i++) {
        if(us % 2 == 0) {
            ret = '0' + ret;
        } else {
            ret = '1' + ret;
        }
        us >>= 1;
    }
    return ret;
}

std::string itobs(int bint) {
    std::string ret = "";
    for(size_t i = 0; i < 32; i++) {
        if((bint & 0b1) == 0) {
            ret = '0' + ret;
        } else {
            ret = '1' + ret;
        }
        bint >>= 1;
    }
    return ret;
}

cpu::cpu() {
    for(size_t i = 0; i < MEMSIZE; i++) {
        memory[i] = i + 5;
    }
}

void cpu::decode(std::string instruction) {
    if(instruction.length() != 32) {
        throw std::invalid_argument("Instruction is not 32 bits");
    }
    ir.operation = bstous(instruction.substr(0,6));
    if(ir.operation != 35 and ir.operation != 43) {
        std::cout << ir.operation;
        throw std::invalid_argument("Instruction Opcode not supported");
    }
    ir.rs = bstous(instruction.substr(6,5));
    if((ir.rs & (0b11000)) == 0b10000) {
        ir.rs &= 0b111;
        ir.rs += 1;
    }
    else if (ir.rs != 0) {
        throw std::invalid_argument("Instruction rs not supported");
    }
    ir.rt = bstous(instruction.substr(11,5));
    if((ir.rt & 0b11000) == 0b10000) {
        ir.rt &= 0b111;
        ir.rt += 1;
    }
    else {
        throw std::invalid_argument("Instruction rt not supported");
    }
    ir.offset = bstous(instruction.substr(16,16));
};

void cpu::compute_address() {
    mem_addr = (unsigned int)registers[ir.rs] + ir.offset;
    mem_addr >>= 2;
    if(mem_addr > MEMSIZE) {
        throw std::invalid_argument("Segmentation fault");
    }
}

block* cpu::determine_set() {
    index = mem_addr % SETSIZE;
    tag = mem_addr / SETSIZE;
    return cache[index];
}

void cpu::run(std::string instruction) {
    block* search_set;
    block* hit_block;
    bool found = false;

    decode(instruction);
    compute_address();
    search_set = determine_set();

    hit_block = &search_set[0];
    if(hit_block->valid and hit_block->tag == tag) {
        found = true;
        lru[index] = true;
    } else if(search_set[1].valid and search_set[1].tag == tag) {
        hit_block = &search_set[1];
        found = true;
        lru[index] = false;
    }

    //Load
    if(ir.operation == 35) {
        if(found) {
            std::cout << "hit";
            registers[ir.rt] = hit_block->data;
        }
        else {
            std::cout << "miss";
            if(lru[index]) {
                hit_block = &search_set[1];
            }
            lru[index] = !lru[index];
            hit_block->valid = true;
            if(hit_block->wb) {
                memory[(hit_block->tag * SETSIZE) + index] = hit_block->data;
                hit_block->wb = false;
            }
            hit_block->tag = tag;
            registers[ir.rt] = memory[mem_addr];
            hit_block->data = registers[ir.rt];
        }
    }
    //Store
    else {
        if(found) {
            std::cout << "hit";
            hit_block->data = registers[ir.rt];
            hit_block->wb = true;
        }
        else {
            std::cout << "miss";
            memory[mem_addr] = registers[ir.rt];
        }
    }
}

void cpu::print_registers() {
    std::cout << "Registers" << std::endl;
    for(size_t i = 0; i < REGSIZE; i++) {
        std::cout << "$S" << i << '\t' << itobs(registers[i+1]) << std::endl;
    }
}

void cpu::print_cache() {
    std::cout << "Cache Block 0\t\t\t\t\t\t\tCache Block 1" << std::endl;
    std::cout << "Set#\tValid\tTag\tData\t\t\t\t\tSet#\tValid\tTag\tData" << std::endl;
    for(size_t i = 0; i < SETSIZE; i++) {
        std::cout << i << '\t' <<
            cache[i][0].valid << '\t' <<
            ustobs(cache[i][0].tag, 4) << '\t' <<
            itobs(cache[i][0].data) << '\t';
        std::cout << i << '\t' <<
            cache[i][1].valid << '\t' <<
            ustobs(cache[i][1].tag, 4) << '\t' <<
            itobs(cache[i][1].data) << std::endl;
    }
}

void cpu::print_memory() {
    std::cout << "Addr\tData" << std::endl;
    for(size_t i = 0; i < MEMSIZE; i++) {
        std::cout << i << '\t' << itobs(memory[i]) << std::endl;
    }
}
