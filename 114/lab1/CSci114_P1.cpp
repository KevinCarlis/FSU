// To run this program you need to compile it using c++11

// Place a data.in file into the same directory as the executable
// then run the executable and a data.out file will be made in place.


// The relative addressing is super fragile. I know Windows changes your
// working directory when you elevate permissions so this obviously wouldn't
// work there, but this is written to spec and no one wants to have to type
// or copy/paste file addresses anyway.

#include <iostream>
#include <fstream>
using namespace std;

int main() {
    int length, offset;
    int blockSize = 0x1000;
    char * memblock = new char[blockSize];
    ifstream inFile ("./data.in", ios::in | ios::binary | ios::ate);
    ofstream outFile("./data.out", ios::out | ios::binary);

    if (inFile and outFile) {
        offset = inFile.tellg() % blockSize;
        length = inFile.tellg() / blockSize;
        inFile.seekg(0, inFile.beg);
        while (length > 0) {
            inFile.read(memblock, blockSize);
            outFile.write(memblock, blockSize);
            length -= 1;
        }
        inFile.read(memblock, offset);
        outFile.write(memblock, offset);
        delete memblock;
        cout << "Copy complete" << endl;
    } else {
        cout << "File Not Found" << endl;
        return 0;
    }
}
