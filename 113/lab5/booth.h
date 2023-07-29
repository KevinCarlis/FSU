#include <iostream>
#include <bitset>
using namespace std;

bool alu_1(bool a, bool b, bool bInvert, bool cIn, bool& cOut) {
    b = b xor bInvert;
    cOut = (((a xor b) and cIn) or (a and b));
    return ((a xor b) xor cIn);
};

void alu_16(bitset<16>& rd, bitset<16> rs, bitset<16> rt, bool sub) {
    bool c = sub;
    for (size_t i = 0; i < 16; i++) {
        rd[i] = alu_1(rs[i], rt[i], sub, c, c);
    }
};

void decrement(bitset<4>& cc) {
    bool c = false;
    for (size_t i = 0; i < 4; i++) {
        cc[i] = alu_1(cc[i], true, false, c, c);
    }
};


class Multiplier {
    bitset<16> md;
    bitset<16> mq;
    bitset<16> ac;
    bitset<4>  cc;
    bool mqr;

    void print();
    public:
        void run(string x, string y);
};

void Multiplier::print() {
    cout << "     " << cc << "     ";
    cout << md << ' ';
    cout << ac << ' ';
    cout << mq << ' ';
    cout << ' ' << mqr << ' ' <<endl;
};

void Multiplier::run(string x, string y) {
    md = bitset<16>{x};
    mq = bitset<16>{y};
    ac = 0b0;
    cc = 0b1111;
    mqr = 0;

    cout << "cycle-counter ";
    cout << "       MD        ";
    cout << "       AC        ";
    cout << "       MQ        ";
    cout << "MQ\u208B\u2081" << endl;

    print();
    while (true) {
        if (mqr xor mq[0]) {
            alu_16(ac, ac, md, mq[0]);
        }
        print();

        mqr = mq[0];
        mq >>= 1;
        mq[15] = ac[0];
        ac >>= 1;
        ac[15] = ac[14];
        print();

        if (cc == 0) {
            break;
        }
        decrement(cc);
    }
}
