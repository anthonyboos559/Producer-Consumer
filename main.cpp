#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unistd.h>

std::mutex buffer_mtx, producer_sleep, consumer_sleep_mtx;
std::condition_variable items, spaces;
std::queue<int> buffer;
const int max_Size;
int producer_sleep, consumer_sleep;
bool shutdown = false;

void consumer();
void producer();

int main(int argc, char *argv[]) {

    // Check for args
    
    // Convert and validate args

    // Setup buffer and control variables

    // Spawn threads

    // Loop on control inputs

    // Cleanup and join threads

    return 0;
}

void consumer() {

    // Begin inf loop

    // Lock sleep and sleep for sleep_time

    // Wait on items or shutdown

    // Check shutdown

    // Consume item from buffer

}

void producer() {

    // Begin inf loop

    // Lock sleep and sleep for sleep_time

    // Wait on spaces or shutdown

    // Check shutdown

    // Produce item for buffer

}