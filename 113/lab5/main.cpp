#include "booth.h"
#include <iostream>
using namespace std;

int main() {
    Multiplier m;
    string md, mq;

    while (cin >> md >> mq) {
        if (md.length() != 16 or mq.length() != 16) { return -1; }
        for (size_t i = 0; i < md.length(); i++)  {
            if ((md[i] != '0' and md[i] != '1') or
                (mq[i] != '0' and mq[i] != '1')) {
                return -1;
            }
        }
        cout << endl;
        m.run(md, mq);
        cout << endl;
    }
    return 0;
}
