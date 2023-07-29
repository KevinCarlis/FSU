// Compiled
// g++ -pthread -std=c++11 -03 -flto -o P3.exe CSci114_P3.cpp

// Usage    M N K
// ./P3.exe 4 5 6

// Requires cin abled matrices in A.txt and B.txt
// A.txt must be M x N and B.txt N x K


// I don't think we even need mutex as we never write to the same location
//#include <mutex>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cassert>


class Matrix
{
    int M, N;
    std::vector<std::vector<int>> data;
//    std::mutex mt;
public:
    ////////////////////////////////////////////////
    ////// Boring constructor operation stuff //////
    ////////////////////////////////////////////////
    // Constructor
    Matrix(int a, int b) :M(a), N(b)
    {
        assert((long long)a*b<0x100000000);
        assert(a>0);
        assert(b>0);
        data = std::vector<std::vector<int>>(M, std::vector<int>(N, 0));
    }
    // Copy Constructor
    Matrix(const Matrix& other)
    {
        N = other.N;
        M = other.M;
        data = std::vector<std::vector<int>>(M, std::vector<int>(N, 0));
        copy(other.data.begin(), other.data.end(), data.begin());
    }
    // Unused Copy Assignment
    Matrix& operator=(const Matrix& other)
    {
        if (this == &other)
            return *this;
        M = other.M;
        N = other.N;
        copy(other.data.begin(), other.data.end(), data.begin());
        return *this;
    }
    // Move Assignment
    Matrix& operator=(Matrix&& other) noexcept
    {
        if (this == & other)
            return *this;

        M = other.M;
        N = other.N;
        data = std::move(other.data);
        return *this;
    }
    // Write value to Matrix
    int& operator()(int a, int b)
    {
        assert(a>=0 && a<M);
        assert(b>=0 && b<N);
        return data.at(a).at(b);
    }
    // Get value from Matrix
    int operator()(int a, int b) const
    {
        assert(a>=0 && a<M);
        assert(b>=0 && b<N);
        return data.at(a).at(b);
    }

    // Print tabular
    void Print()
    {
        for (int i=0; i<M; i++) {
            for(int j=0; j<N; j++)
                std::cout << data.at(i).at(j) << '\t';
            std::cout << std::endl;
        }
    }


    ////////////////////////////////////////////////
    //////////////// THREAD STUFF //////////////////
    ////////////////////////////////////////////////

    // Thread function multiplies row A by col B
    void multiplyThread(int x, int y, Matrix A, Matrix B)
    {
//        mt.lock();
        for (int i=0; i<A.N; i++)
            this->operator()(x,y) += A(x,i) * B(i,y);
//        mt.unlock();
    }

    // THREAD MAIN
    Matrix& operator*=(const Matrix& rhs)
    {
        assert(N == rhs.M);
        Matrix ret(M, rhs.N);
        std::thread myThreads[M][rhs.N];
        for (int i=0; i<M; i++)
            for(int j=0; j<rhs.N; j++)
                myThreads[i][j] = std::thread(
                        &Matrix::multiplyThread, &ret,
                        i, j, std::ref(*this), std::ref(rhs));
        for (int i=0; i<M; i++)
            for(int j=0; j<rhs.N; j++)
                myThreads[i][j].join();
        *this = std::move(ret);
        return *this;
    }
    // Ditto ^
    friend Matrix operator*(Matrix lhs, const Matrix& rhs)
    {
        lhs *= rhs;
        return lhs;
    }
};

int main(int argc, char** argv)
{
    assert(argc >= 4);

    int M, N, K;
    M = atoi(argv[1]);
    N = atoi(argv[2]);
    K = atoi(argv[3]);

    Matrix A(M,N);
    std::ifstream inA("A.txt", std::ios::in);
    for (int i=0; i<M; i++)
        for(int j=0; j<N; j++)
            inA >> A(i,j);
    
    Matrix B(N,K);
    std::ifstream inB("B.txt", std::ios::in);
    for (int i=0; i<N; i++)
        for(int j=0; j<K; j++)
            inB >> B(i,j);

    A *= B;
    A.Print();

    return 0;
}
