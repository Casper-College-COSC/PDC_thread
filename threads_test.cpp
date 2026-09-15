#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
#include <cmath>

// Increase these to make it run longer
const long long ARRAY_SIZE  = 200000000LL;  // 200 million elements
const int       NUM_THREADS = 4;

// -------------------------------------------------------
// Worker function — does extra math per element so
// threads stay alive long enough to see in Task Manager
// -------------------------------------------------------
void fillArrayTask(long long* arr, long long start, long long end, int id) {
    printf("Thread %d starting  (indices %lld to %lld)\n", id, start, end - 1);

    for (long long i = start; i < end; ++i) {
        // Extra work: sqrt slows each iteration down
        // so threads stay visible longer in Task Manager
        arr[i] = (long long)sqrt((double)i) + i;
    }

    printf("Thread %d finished.\n", id);
}

int main() {

    std::cout << "Hardware concurrency: "
              << std::thread::hardware_concurrency()
              << " logical processors" << std::endl;

    std::cout << "\nAllocating " << ARRAY_SIZE
              << " elements (~"
              << (ARRAY_SIZE * sizeof(long long)) / (1024 * 1024 * 1024)
              << " GB RAM)..." << std::endl;

    // Allocate on the heap
    std::vector<long long> data(ARRAY_SIZE, 0);

    std::cout << "Allocation complete. Starting threads...\n" << std::endl;
    std::cout << ">>> Open Task Manager NOW and find this process <<<\n" << std::endl;

    // Small pause so you can switch to Task Manager
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Pausing, hit any key to continue" << std::endl;
    std::cin.get(); 

    // -------------------------------------------------------
    // SEQUENTIAL FILL
    // -------------------------------------------------------
    std::cout << "--- Sequential Fill ---" << std::endl;
    auto startSeq = std::chrono::high_resolution_clock::now();

    for (long long i = 0; i < ARRAY_SIZE; ++i) {
        data[i] = (long long)sqrt((double)i) + i;
    }

    auto endSeq   = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeq = endSeq - startSeq;
    std::cout << "Sequential time: " << elapsedSeq.count() << " seconds\n" << std::endl;

    // -------------------------------------------------------
    // PARALLEL FILL
    // -------------------------------------------------------
    std::cout << "--- Parallel Fill (4 threads) ---" << std::endl;
    std::cout << ">>> Watch the thread count jump in Task Manager <<<\n" << std::endl;
    std::cout << ">>> Switch to Activity Monitor NOW <<<" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    long long chunk = ARRAY_SIZE / NUM_THREADS;

    auto startPar = std::chrono::high_resolution_clock::now();

    std::thread t1(fillArrayTask, data.data(), 0,         chunk,     1);
    std::thread t2(fillArrayTask, data.data(), chunk,     chunk * 2, 2);
    std::thread t3(fillArrayTask, data.data(), chunk * 2, chunk * 3, 3);
    std::thread t4(fillArrayTask, data.data(), chunk * 3, ARRAY_SIZE,4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    auto endPar   = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedPar = endPar - startPar;
    std::cout << "\nParallel time:   " << elapsedPar.count() << " seconds" << std::endl;

    // -------------------------------------------------------
    // RESULTS
    // -------------------------------------------------------
    double speedup = elapsedSeq.count() / elapsedPar.count();
    std::cout << "\nPredicted speedup (ideal): " << NUM_THREADS << "x" << std::endl;
    std::cout << "Actual speedup:            " << speedup     << "x" << std::endl;

    std::cin.get();

    return 0;
}