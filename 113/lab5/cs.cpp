#include<iostream>
#include<bitset>
using namespace std;

//1 bit add/sub ALU
//returns sum, fills carry out as reference
bool alu_1(bool a, bool b, bool bInvert,
           bool cI, bool& cO) {
    b = b xor bInvert;
    cO = (((a xor b) and cI) or (a and b));
    return ((a xor b) xor cI);
}

//Booth multiplier
class Multiplier {
    bitset<16> md;
    bitset<16> mq;
    bitset<16> ac;
    bitset<4> cc;
    bool mqr;
    void alu_16(
        bitset<16>& rd,
        bitset<16> rs,
        bitset<16> rt,
        bool sub
    );
    void decrement_counter();
    void print();
    public:
        void multiply(string x, string y);
};

//16 bit add/sub ALU
//Sets first carry out to sub signal
void Multiplier::alu_16(
    bitset<16>& rd,
    bitset<16> rs,
    bitset<16> rt,
    bool sub
) {
    bool c = sub;
    for (size_t i = 0; i < 16; i++) {
        rd[i] = alu_1(rs[i], rt[i], sub, c, c);
    }
};

//Subtracts 1 from CC by adding -1 0b1111 directly
void Multiplier::decrement_counter() {
    bool c = false;
    for (size_t i = 0; i < 4; i++) {
        cc[i] = alu_1(cc[i], true, false, c, c);
    }
};

void Multiplier::print() {
    cout << "    " << cc << "      ";
    cout << md << " ";
    cout << ac << " ";
    cout << mq << "  ";
    cout << mqr << endl;
};

void Multiplier::multiply(string x, string y) {
    md = bitset<16> {x};
    mq = bitset<16> {y};
    mqr = 0;
    ac = 0;
    cc = 0b1111;

    cout << "cycle-counter        md               ";
    cout << "ac               mq        mq\u208B\u2081\n";
    print();

    while (true) {
        //Add multiplicand to accumulator if mq[0,-1] = 0b01
        //Subtract to accumulator if mq[0,-1] = 0b10
        if (mq[0] xor mqr) {
            alu_16(ac, ac, md, mq[0]);
        }
        print();

        //Arithmetic Right Shift
        mqr = mq[0];
        mq >>= 1;
        mq[15] = ac[0];
        ac >>= 1;
        ac[15] = ac[14];
        print();
        if (cc == 0) { break; }
        decrement_counter();
    }
};

int main() {
    Multiplier m;
    string md, mq;
    cout << "Enter two binary 16 bit numbers\n";
    cout << "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX\n";
    while (cin >> md >> mq) {
        if (md.length() != 16 or mq.length() != 16) { return 0; }
        for (size_t i = 0; i < md.length(); i++)  {
            if ((md[i] != '0' and md[i] != '1') or
                (mq[i] != '0' and mq[i] != '1')) {
                return 0;
            }
        }
        cout << endl;
        m.multiply(md, mq);
        cout << "\nEnter two binary 16 bit numbers\n";
        cout << "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX\n";
    }
    return 0;
}
