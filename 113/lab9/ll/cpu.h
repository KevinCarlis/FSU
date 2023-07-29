#pragma once
#include <bitset>
#include <cmath>

const unsigned short BLOCKSIZE = 2;
const unsigned short CACHESIZE = 8;
const unsigned short REGSIZE = 8;
const unsigned short MEMSIZE = 128;
const unsigned short INDEXSIZE = std::ceil(std::log(CACHESIZE)/std::log(2));
const unsigned short TAGSIZE = std::ceil(std::log(MEMSIZE)/std::log(2)) - INDEXSIZE;

const unsigned short WORDSIZE = 32;
using Word = std::bitset<WORDSIZE>;

class block {
public:
    unsigned short set = 0;
    bool valid;
    std::bitset<TAGSIZE> tag;
    Word data;
    block* next = NULL;
};

class set {
public:
    block* mru;
    set();
    block* load_search(std::bitset<TAGSIZE> tag);
    block* store_search(std::bitset<TAGSIZE> tag);
    void store(Word new_data);
};

class instruction_register {
public:
    std::bitset<6> operation;
    std::bitset<5> rs;
    std::bitset<5> rt;
    std::bitset<16> offset;
    void decode(std::string instruction);
    void validate();
};

class cpu {
    instruction_register ir;
    Word registers[REGSIZE + 1]; //[0] = $zero
    Word memory[MEMSIZE];
    set cache[CACHESIZE];
    Word word_addr;
    std::bitset<INDEXSIZE> index;
    std::bitset<TAGSIZE> tag;
    void compute_address();
    set determine_set();
public:
    cpu();
    void run(std::string instruction);
};
