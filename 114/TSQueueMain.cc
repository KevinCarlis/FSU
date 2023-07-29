#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

// Thread-safe queue interface
 
 const int MAX = 10;
 
 class TSQueue {
   // Synchronization variables
     mutex m;
 
   // State variables
     int items[MAX];
     int front;
     int nextEmpty;
 
   public:
     TSQueue();
     ~TSQueue(){};
     bool tryInsert(int item);
     bool tryRemove(int *item);
 };

 // Initialize the queue to empty
 // and the lock to free.
 TSQueue::TSQueue() {
     front = nextEmpty = 0;
 }
 
 // Try to insert an item. If the queue is
 // full, return false; otherwise return true.
 bool
 TSQueue::tryInsert(int item) {
     bool success = false;
 
     lock_guard<mutex> lock(m);
     if ((nextEmpty - front) < MAX) {
         items[nextEmpty % MAX] = item;
         nextEmpty++;
         success = true;
     }
     return success;
 }
 
 // Try to remove an item. If the queue is
 // empty, return false; otherwise return true.
 bool
 TSQueue::tryRemove(int *item) {
     bool success = false;
 
     lock_guard<mutex> lock(m);
     if (front < nextEmpty) {
         *item = items[front % MAX];
         front++;
         success = true;
     }
     return success;
 }
 
 // Insert 50 items into a queue.
 void *putSome(void *p) {
     TSQueue *queue = (TSQueue *)p;
     int i;
 
     for (i = 0; i < 50; i++) {
         queue->tryInsert(i);
     }
     return NULL;
 }
 
 // Remove 20 items from a queue.
 void testRemoval(TSQueue *queue) {
     int i, item;
 
     for (i = 0; i < 20; i++) {
         if (queue->tryRemove(&item))
           printf("Removed %d\n", item);
         else
           printf("Nothing there.\n");
     }
 }
 

// TSQueueMain.cc
//   Test code for TSQueue.
 
 int main(int argc, char **argv) {
     TSQueue *queues[3];
     thread workers[3];
     int i;
 
     // Start worker threads to insert.
     for (i = 0; i < 3; i++) {
         queues[i] = new TSQueue();
         workers[i] = thread(&putSome, queues[i]);
     }
 
     // Wait for some items to be put.
     workers[0].join();
 
     // Remove 20 items from each queue.
     for (i = 0; i < 3; i++) {
         printf("Queue %d:\n", i);
         testRemoval(queues[i]);
     }
     workers[1].join();
     workers[2].join();
     return 0;
 }
