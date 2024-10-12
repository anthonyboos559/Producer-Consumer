#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
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

    // Loop on control inputs
    char command;
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

void consume_item(int *bin) {

    int val = buffer.front();
    buffer.pop();
    std::cout << "\t\t\tGet " << val << " from bin " << *bin << std::endl;
    (*bin)++;
    if (*bin == buffer_size) {
        *bin = 0;
    }

}

void consumer_loop() {
    
    int bin = 0;
    while (true) {
        // Lock sleep and sleep for sleep_time
        std::lock_guard<std::mutex> sleep_lock(consumer_sleep_mtx);
        std::this_thread::sleep_for(std::chrono::milliseconds(consumer_sleep));

        // Wait on items or shutdown
        std::unique_lock<std::mutex> lock(buffer_mtx);
        items.wait(lock, [] { return !buffer.empty() || shutdown; });

        // Flush the buffer if shutdown has initiated
        if (shutdown) {
            while (!buffer.empty()) {
                consume_item(&bin);
            }
            break;
        }
        // Else just consume an item from buffer
        else {
            consume_item(&bin);
            spaces.notify_one();
        }
    }
    std::cout << "End of Consumer." << std::endl;

}

void produce_item(int *bin) {

    int val = std::rand() % 10000;
    buffer.push(val);
    std::cout << "Put " << val << " into bin " << *bin << std::endl;
    (*bin)++;
    if (*bin == buffer_size) {
        *bin = 0;
    }

}

void producer_loop() {

    int bin = 0;
    while (true) {
        // Lock sleep and sleep for sleep_time
        std::lock_guard<std::mutex> sleep_lock(producer_sleep_mtx);
        std::this_thread::sleep_for(std::chrono::milliseconds(producer_sleep));

        // Wait on items or shutdown
        std::unique_lock<std::mutex> lock(buffer_mtx);
        spaces.wait(lock, [] { return buffer.size() < buffer_size || shutdown; });

        // Stop production if shutdown has initiated
        if (shutdown) {
            break;
        }
        // Else just produce an item for the buffer
        else {
            produce_item(&bin);
            items.notify_one();
        }
    }
    std::cout << "End of Producer." << std::endl;

}

int validate_and_set_args(int argc, char *argv[]) {
    
    // Check for correct amount of args
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