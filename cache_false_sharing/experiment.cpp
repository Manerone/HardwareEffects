/// Thanks to https://coffeebeforearch.github.io/2019/12/28/false-sharing-tutorial.html
/// for a nice explanation of this.

#include <benchmark/benchmark.h>
#include <atomic>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>

/// Shared state used to keep track of all
/// the different counters
struct SharedState {
    std::atomic<int64_t> counter0;
    std::atomic<int64_t> counter1;
    std::atomic<int64_t> counter2;
    std::atomic<int64_t> counter3;
};

/// Table with columns that will be used to count
struct Table {
    std::vector<std::vector<int64_t>> columns;

    Table(){
        columns.resize(4);
        for(auto& col : columns){
            col.resize(1000000);
            for(size_t i = 0; i < col.size(); i++){
                col[i] = i;
            }
        }
    }
};

/// Method used to apply the filters and count the occurrence in a column
void filter_and_count(std::vector<int64_t> &column, std::atomic<int64_t> &counter, std::function<bool(int64_t)> &&filter){
    for(const auto value : column){
        if(filter(value)){
            counter++;
        }
    }
}

/// Using a single thread, filter and count the occurrences
static void BM_SINGLE_THREAD(benchmark::State& state){
    Table table;
    while (state.KeepRunning()) {
        SharedState shared_state;

        filter_and_count(table.columns[0], shared_state.counter0, [](int64_t v){
            return v % 2 == 0;
        });

        filter_and_count(table.columns[1], shared_state.counter1, [](int64_t v){
            return v % 3 == 0;
        });

        filter_and_count(table.columns[2], shared_state.counter2, [](int64_t v){
            return v % 4 == 0;
        });

        filter_and_count(table.columns[3], shared_state.counter3, [](int64_t v){
            return v % 5 == 0;
        });
    }
}

/// Using a multiple threads, filter and count the occurrences, but shared state
/// will occupy the same cache line
static void BM_NAIVE_THREAD(benchmark::State& state){
    Table table;
    while (state.KeepRunning()) {
        SharedState shared_state;

        std::thread t1([&](){
            filter_and_count(table.columns[0], shared_state.counter0, [](int64_t v){
                return v % 2 == 0;
            });
        });

        std::thread t2([&](){
            filter_and_count(table.columns[1], shared_state.counter1, [](int64_t v){
                return v % 3 == 0;
            });
        });

        std::thread t3([&](){
            filter_and_count(table.columns[2], shared_state.counter2, [](int64_t v){
                return v % 4 == 0;
            });
        });

        std::thread t4([&](){
            filter_and_count(table.columns[3], shared_state.counter3, [](int64_t v){
                return v % 5 == 0;
            });
        });

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}

/// Using multiple threads, but each thread has a local variable to count
/// and only then we move the result to the shared state
static void BM_LOCAL_VARIABLES(benchmark::State& state){
    Table table;
    while (state.KeepRunning()) {
        SharedState shared_state;

        std::thread t1([&](){
            std::atomic<int64_t> local;
            filter_and_count(table.columns[0], local,
                             [](int64_t v) { return v % 2 == 0; });
            shared_state.counter0 = local.load();
        });

        std::thread t2([&](){
            std::atomic<int64_t> local;
            filter_and_count(table.columns[1], local,
                             [](int64_t v) { return v % 3 == 0; });
            shared_state.counter1 = local.load();
        });

        std::thread t3([&]() {
          std::atomic<int64_t> local;
          filter_and_count(table.columns[2], local,
                           [](int64_t v) { return v % 4 == 0; });
          shared_state.counter2 = local.load();
        });

        std::thread t4([&](){
            std::atomic<int64_t> local;
            filter_and_count(table.columns[3], local, [](int64_t v){
                return v % 5 == 0;
            });
             shared_state.counter3 = local.load();
        });

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}

/// Same struct as SharedState, but with the counters
/// aligned to 64 byte addresses
struct AlignedSharedState {
    alignas(64) std::atomic<int64_t> counter0;
    alignas(64) std::atomic<int64_t> counter1;
    alignas(64) std::atomic<int64_t> counter2;
    alignas(64) std::atomic<int64_t> counter3;
};

/// Same code as the naive multi-threaded, but using aligned structs
static void BM_ALIGNED_COUNTERS(benchmark::State& state){
    Table table;
    while (state.KeepRunning()) {
        AlignedSharedState shared_state;

        std::thread t1([&](){
            filter_and_count(table.columns[0], shared_state.counter0, [](int64_t v){
                return v % 2 == 0;
            });
        });

        std::thread t2([&](){
            filter_and_count(table.columns[1], shared_state.counter1, [](int64_t v){
                return v % 3 == 0;
            });
        });

        std::thread t3([&](){
            filter_and_count(table.columns[2], shared_state.counter2, [](int64_t v){
                return v % 4 == 0;
            });
        });

        std::thread t4([&](){
            filter_and_count(table.columns[3], shared_state.counter3, [](int64_t v){
                return v % 5 == 0;
            });
        });

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}

BENCHMARK(BM_SINGLE_THREAD)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_NAIVE_THREAD)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LOCAL_VARIABLES)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ALIGNED_COUNTERS)->UseRealTime()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();