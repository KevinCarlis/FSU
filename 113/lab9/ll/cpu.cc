#include <iostream>
#include <cmath>
#include "cpu.h"

set::set() {
    mru = new block;
    block* prev = mru;
    block* curr;
    for (size_t i = 1; i < BLOCKSIZE; i++) {
        curr = new block;
        curr->set = (unsigned short)i;
        prev->next = curr;
        prev = curr;
    }
}

block* set::load_search(std::bitset<TAGSIZE> tag) {
    block* curr = mru;
    block* prev = mru;
    while(curr->next) {
        if(!curr->valid || curr->tag == tag) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if(curr != mru) {
        prev->next = curr->next;
        curr->next = mru;
        mru = curr;
    }
    return mru;
}

block* set::store_search(std::bitset<TAGSIZE> tag) {
    block* curr = mru;
    block* prev = mru;
    while(curr->next) {
        if(!curr->valid) {
            return NULL;
        }
        if(curr->tag == tag) {
            if(curr != mru) {
                prev->next = curr->next;
                curr->next = mru;
                mru = curr;
            }
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }
}

void set::store(Word new_data) {
}

cpu::cpu() {
    if (TAGSIZE + INDEXSIZE > WORDSIZE) {
        throw std::invalid_argument("Addressing too large for word size");
    }
    for(size_t i = 0; i < MEMSIZE; i++) {
        memory[i] = i + 5;
    }
}

void instruction_register::decode(std::string instruction) {
    operation = std::bitset<6>{instruction.substr(0,6)};
    rs = std::bitset<5>{instruction.substr(6,5)};
    rt = std::bitset<5>{instruction.substr(11,5)};
    offset = std::bitset<16>{instruction.substr(16,16)};
    validate();
};

void instruction_register::validate() {
    if((operation & std::bitset<6>(0b110111)) != 0b100011) {
        throw std::invalid_argument("Instruction Opcode not supported");
    }
    if(rs != 0 && (rs & std::bitset<5>(0b11000)) != 0b10000) {
        throw std::invalid_argument("Instruction rs not supported");
    }
    if((rt & std::bitset<5>(0b11000)) != 0b10000) {
        throw std::invalid_argument("Instruction rt not supported");
    }
}

void cpu::compute_address() {
    size_t rs;
    rs = (ir.rs & std::bitset<5>(0b00111)).to_ulong();
    rs += ((ir.rs & std::bitset<5>(0b10000)) != 0);
    word_addr = Word(registers[rs].to_ulong() + ir.offset.to_ulong());
    word_addr >>= 2;
    if(word_addr.to_ulong() > MEMSIZE) {
        throw std::invalid_argument("Segmentation fault");
    }
}

set cpu::determine_set() {
    for (size_t i = 0; i < INDEXSIZE; i++) {
        index[i] = word_addr[i];
    }
    for (size_t i = 0; i < TAGSIZE; i++) {
        tag[i] = word_addr[i + INDEXSIZE];
    }
    return cache[index.to_ulong()];
}

void cpu::run(std::string instruction) {
    if(instruction.length() != WORDSIZE) {
        throw std::invalid_argument("Instruction must be " +
                std::to_string(WORDSIZE) +
                " bits"
                );
    }
    set hit_set;
    block* hit_block;

    ir.decode(instruction);
    compute_address();
    hit_set = determine_set();

    size_t rt;
    rt = (ir.rt & std::bitset<5>(0b00111)).to_ulong();
    rt += ((ir.rt & std::bitset<5>(0b10000)) != 0);
   
    //Load
    if((ir.operation & std::bitset<6>(0b001000)) == 0) {
        hit_block = hit_set.load_search(tag);
        if(hit_block->valid && (hit_block->tag == tag)) {
            std::cout << "hit";
            registers[rt] = hit_block->data;
        }
        else {
            std::cout << "miss";
            registers[rt] = memory[word_addr.to_ulong()];
            hit_block->data = registers[rt];
        }
    }
    //Store
    else {
        hit_block = hit_set.store_search(tag);
        if(hit_block) {
            std::cout << "hit";
            hit_block->data = registers[rt];
        }
        else {
            std::cout << "miss";
            memory[word_addr.to_ulong()] = registers[rt];
        }
    }
};
