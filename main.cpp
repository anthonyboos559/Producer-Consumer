#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unistd.h>

std::mutex buffer_mtx, producer_sleep_mtx, consumer_sleep_mtx;
std::condition_variable items, spaces;
std::queue<int> buffer;
int buffer_size, producer_sleep, consumer_sleep;
bool shutdown = false;

void consumer_loop();
void producer_loop();
int validate_and_set_args(int argc, char *argv[]);

int main(int argc, char *argv[]) {

    // Setup buffer and control variables
    if (validate_and_set_args(argc, argv) > 0) {
        return 1;
    }

    // Spawn threads
    std::thread producer(producer_loop);
    std::thread consumer(consumer_loop);

    char command;
    // Loop on control inputs
    while (!shutdown) {
        std::cin >> command;
        switch (command)
        {
        case 'a': {
            std::lock_guard<std::mutex> lock(producer_sleep_mtx);
            producer_sleep += 250;
            break;
        }
        case 'z': {
            std::lock_guard<std::mutex> lock(producer_sleep_mtx);
            producer_sleep -= 250;
            break;
        }
        case 's': {
            std::lock_guard<std::mutex> lock(consumer_sleep_mtx);
            consumer_sleep += 250;
            break;
        }
        case 'x': {
            std::lock_guard<std::mutex> lock(consumer_sleep_mtx);
            consumer_sleep -= 250;
            break;
        }
        case 'q': {
            std::cout << "Preparing to shutdown." << std::endl;
            shutdown = true;
            items.notify_all();
            spaces.notify_all();
            break;
        }
        default:
            break;
        }
    }

    // Cleanup and join threads
    producer.join();
    consumer.join();

    std::cout << "End of main." << std::endl;

    return 0;
}

void consumer_loop() {

    // Begin inf loop

    // Lock sleep and sleep for sleep_time

    // Wait on items or shutdown

    // Check shutdown

    // Consume item from buffer

}

void producer_loop() {

    // Begin inf loop

    // Lock sleep and sleep for sleep_time

    // Wait on spaces or shutdown

    // Check shutdown

    // Produce item for buffer

}

int validate_and_set_args(int argc, char *argv[]) {
    
    // Check for correct ammount of args
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <buffer_size> <producer_sleep> <consumer_sleep>" << std::endl;
        return 1;
    }
    
    // Convert, assign, and validate the args
    try {
        buffer_size = std::stoi(argv[1]);
        if (buffer_size < 0) {
            throw std::invalid_argument("Error: buffer_size cannot be negative.");
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return 1; 
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    try {
        producer_sleep = std::stoi(argv[2]);
        if (producer_sleep < 0) {
            throw std::invalid_argument("Error: producer_sleep cannot be negative.");
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return 1; 
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    try {
        consumer_sleep = std::stoi(argv[3]);
        if (consumer_sleep < 0) {
            throw std::invalid_argument("Error: consumer_sleep cannot be negative.");
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return 1; 
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}