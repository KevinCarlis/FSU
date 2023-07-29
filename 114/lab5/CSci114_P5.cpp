// Compilation
// g++ CSci114_P5.cpp --std=c++11 -flto -O3 -o P5.exe

// Block id is from 1 to 1024


#include <iostream>
#include <fstream>


class FreeNode {
    public:
        int id, freeMem = 1024;
        FreeNode *next = NULL;
        FreeNode(int i) : id{i} {};
};

class FreeList {
    public:
        FreeNode *head;
        FreeList() {
            FreeNode *curr;
            head = new FreeNode(1);
            curr = head;
            for (int i = 2; i <= 1024; i++) {
                curr->next = new FreeNode(i);
                curr = curr->next;
            }
        }
        ~FreeList() {
            FreeNode *curr, *next;
            curr = head;
            while (curr) {
                next = curr->next;
                delete curr;
                curr = next;
            }
        }
        void Insert(FreeNode* node) {
            FreeNode *curr, *prev = NULL;
            curr = head;
            while (curr) {
                if (curr->freeMem > node->freeMem) {
                    node->next = curr;
                    if (prev)
                        prev->next = node;
                    else
                        head = node;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            if (!curr) {
                prev->next = node;
                node->next = NULL;
            }
        }

        FreeNode* Allocate(int mem) {
            FreeNode *curr, *prev= NULL;
            curr = head;
            while (curr) {
                if (mem <= curr->freeMem) {
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            if (curr) {
                curr->freeMem = curr->freeMem - mem;
                if (curr != head && prev->freeMem > curr->freeMem) {
                    prev->next = curr->next;
                    Insert(curr);
                }
            }
            return curr;
        }

        FreeNode* Free(int id, int mem) {
            FreeNode *curr, *prev= NULL;
            curr = head;
            while (curr) {
                if (curr->id == id) {
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            if (curr) {
                curr->freeMem = curr->freeMem + mem;
                if (curr->freeMem > curr->next->freeMem) {
                    if (prev)
                        prev->next = curr->next;
                    else
                        head = curr->next;
                    Insert(curr);
                }
            }
            return curr;
        }
};


class AllocNode {
    public:
        int id, blockId, mem;
        AllocNode *next = NULL;
        AllocNode(int i, int b, int m) : id{i}, blockId{b}, mem{m} {};
};

class AllocList {
    public:
        AllocNode *head;
        ~AllocList() {
            AllocNode *curr, *next;
            curr = head;
            while (curr) {
                next = curr->next;
                delete curr;
                curr = next;
            }
        }

        void Insert(int id, int blockId, int mem) {
            if (!head)
                head = new AllocNode(id, blockId, mem);
            else {
                AllocNode *curr, *prev = NULL;
                curr = head;
                while (curr) {
                    if (id < curr->id) {
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
                if (prev) {
                    prev->next = new AllocNode(id, blockId, mem);
                    prev->next->next = curr;
                }
                else {
                    head = new AllocNode(id, blockId, mem);
                    head->next = curr;
                }
            }
        }

        AllocNode* Release(int id) {
            AllocNode *curr, *prev = NULL;
            curr = head;
            while (curr) {
                if (curr->id == id)
                    break;
                prev = curr;
                curr = curr->next;
            }
            if (curr) {
                if (prev) {
                    prev->next = curr->next;
                }
                else {
                    head = curr->next;
                }
            }
            return curr;
        }
};



int main() {
    FreeList memList;
    AllocList reqList;
    FreeNode* memNode;
    AllocNode* reqNode;

    char req;
    int rid, memSize;

    std::ifstream inReq("requests-1.txt", std::ios::in);
    while (inReq.good()) {
        inReq >> req;
        memNode = memList.head; 
        if (req == 'A') {
            inReq >> rid;
            inReq >> memSize;
            memNode = memList.Allocate(memSize);
            if (!memNode)
                std::cout << "No free memory. Request not processed.\n";
            else {
                reqList.Insert(rid, memNode->id, memSize);
                std::cout << memSize << " bytes have been allocated at block ";
                std::cout << memNode->id << " for request " << rid << std::endl;
            }
        }
        else if (req == 'R') {
            inReq >> rid;
            reqNode = reqList.Release(rid);
            if (!reqNode)
                std::cout << "Unable to find request " << rid << std::endl;
            else {
                memNode = memList.Free(reqNode->blockId, reqNode->mem);
                if (!memNode) {
                    std::cout << "Unable to find block :" 
                        << reqNode->blockId << std::endl;
                }
                else {
                    std::cout << reqNode->mem 
                        << " bytes have been return to block " 
                        << reqNode->blockId 
                        << " for request " 
                        << rid << std::endl;
                }
                delete reqNode;
            }
        }
        else {
            std::cout << "Invalid operation: " << req << std::endl;
            break;
        }
    }

    std::ofstream outMem("final_size.txt", std::ios::out);
    memNode = memList.head;
    while(memNode) {
        outMem << memNode->freeMem << std::endl;
        memNode = memNode->next;
    }
    return 0;
}
