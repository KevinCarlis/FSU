// Compiled using 
// g++ CSci114_P4.cpp -std=c++11 -pthreads -O3 -flto -o P4.exe 


// I think the car number prints are really slow so I kept them out of the
// locked events so they may be interleavened randomly and leave will always
// print after the tunnel empty notice.

// I was a little confused about your N car waiting rule so I made it so that
// if N+ cars are waiting both directions, they take turns one car at a time.


#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <condition_variable>
#include <thread>


int N;
int NBatchRule = 0;
int waitNorth = 0;
int waitSouth = 0;
int inNorth = 0;
int inSouth = 0;

int direction = -1;
const int NORTH = 0;
const int SOUTH = 1;

std::mutex m;
std::condition_variable northCV;
std::condition_variable southCV;


bool northShouldWait() {
    return (NBatchRule >= N || inNorth >= N || inSouth > 0 ||
            (waitSouth > 0 && direction == SOUTH));
}

bool southShouldWait() {
    return (NBatchRule >= N || inSouth >= N || inNorth > 0 ||
            (waitNorth > 0 && direction == NORTH));
}

void northEnter() {
    std::unique_lock<std::mutex> lock(m);

    waitNorth++;
    while (northShouldWait())
        northCV.wait(lock);

    if (direction != NORTH) {
        std::cout << "No south cars waiting\nDirection changed to north\n";
        direction = NORTH;
        NBatchRule = 0;
    }

    inNorth++;
    waitNorth--;

    // This is hack, if N cars are waiting other direction set NBatchBule to N
    // so no more cars enter and a swap begins. Since it gets incremented, we
    // know that anytime NBatchRule = N + 1 we had a waiting tunnel swap.
    if (waitSouth >= N)
        NBatchRule = N;
    NBatchRule++;

    lock.unlock();
}

void southEnter() {
    std::unique_lock<std::mutex> lock(m);

    waitSouth++;
    while (southShouldWait())
        southCV.wait(lock);

    if (direction != SOUTH) {
        std::cout << "No north cars waiting\nDirection changed to south\n";
        direction = SOUTH;
        NBatchRule = 0;
    }

    inSouth++;
    waitSouth--;

    if (waitNorth >= N)
        NBatchRule = N;
    NBatchRule++;

    lock.unlock();
}

void northLeave() {
    std::lock_guard<std::mutex> lock(m);
    inNorth--;
    if (inNorth == 0) {
        std::cout << "Tunnel Empty\n";
        if(NBatchRule == N)
            std::cout << "Nth car has left the tunnel\n";
        else if (NBatchRule > N)
            std::cout << "N cars waiting south\n";
        if (NBatchRule >= N)
            NBatchRule = 0;

        if (waitSouth > 0) {
            std::cout << "Direction changed south\n";
            direction = SOUTH;
            southCV.notify_all();
        } else
            northCV.notify_all();
    }
}

void southLeave() {
    std::lock_guard<std::mutex> lock(m);
    inSouth--;
    if (inSouth == 0) {
        std::cout << "Tunnel Empty\n";
        if (NBatchRule == N)
            std::cout << "Nth car has left the tunnel\n";
        else if (NBatchRule == N + 1)
            std::cout << "N cars waiting north\n";
        if (NBatchRule >= N)
            NBatchRule = 0;

        if (waitNorth > 0) {
            std::cout << "Direction changed north\n";
            direction = NORTH;
            northCV.notify_all();
        } else 
            southCV.notify_all();
    }
}

void northRun(int carNum) {
    printf("#%d north direction car arriving\n", carNum);
    northEnter();
    printf("#%d north direction car entered the tunnel\n", carNum);
    northLeave();
    printf("#%d north direction car left the tunnel\n", carNum);
}

void southRun(int carNum) {
    printf("#%d south direction car arriving\n", carNum);
    southEnter();
    printf("#%d south direction car entered the tunnel\n", carNum);
    southLeave();
    printf("#%d south direction car left the tunnel\n", carNum);
    return;
}

int main(int argc, char* argv[]) {
    assert(argc > 1);
    N = atoi(argv[1]);
    assert((N >= 5) && (N <= 15));

    char arrival;
    int northCarNum = 1;
    int southCarNum = 1;
    std::vector<std::thread> myThreads;
    std::ifstream carsFile("cars.txt", std::ios::in);

    while (carsFile.good()) {
        carsFile >> arrival;
        assert(arrival == 'N' or arrival == 'S');
        if (arrival == 'N')
            myThreads.push_back(std::thread(&northRun, northCarNum++));
        else
            myThreads.push_back(std::thread(&southRun, southCarNum++));
    }
    std::cout << "\n\nAll cars created\n\n\n";
    for (size_t i = 0; i < myThreads.size(); i++) {
        myThreads.at(i).join();
    }
    return 0;
}
