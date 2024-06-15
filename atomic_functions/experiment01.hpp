/// In this experiment we test the difference in speed when using
/// locks, atomics, CAS.

#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <mutex>

const size_t N = 1'000'000;

static void BM_ATOMIC_ADD(benchmark::State& state) {
    auto f = [](){
        std::atomic<int> x{0};
        for(size_t i = 0; i < N; i++){
            x++;
        }
    };
    while(state.KeepRunning()){
        f();
    }
}

static void BM_LOCK_ADD(benchmark::State& state) {
    auto f = [](){
        int x{0};
        std::mutex m;
        for(size_t i = 0; i < N; i++){
            std::lock_guard<std::mutex> guard(m);
            x++;
        }
    };
    while(state.KeepRunning()){
        f();
    }
}

static void BM_CAS_ADD_WEAK(benchmark::State& state) {
    auto f = [](){
        std::atomic<int> x{0};
        for(size_t i = 0; i < N; i++){
            int v;
            do{
                v = x.load();
            }
            while(!x.compare_exchange_weak(v, x+1));
        }
    };
    while(state.KeepRunning()){
        f();
    }
}

static void BM_CAS_ADD_STRONG(benchmark::State& state) {
    auto f = [](){
        std::atomic<int> x{0};
        for(size_t i = 0; i < N; i++){
            int v;
            do{
                v = x.load();
            }
            while(!x.compare_exchange_strong(v, x+1));
        }
    };
    while(state.KeepRunning()){
        f();
    }
}

static void BM_FETCH_ADD(benchmark::State& state) {
    auto f = [](){
        std::atomic<int> x{0};
        for(size_t i = 0; i < N; i++){
            x.fetch_add(1);
        }
    };
    while(state.KeepRunning()){
        f();
    }
}

BENCHMARK(BM_ATOMIC_ADD)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CAS_ADD_WEAK)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CAS_ADD_STRONG)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_FETCH_ADD)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LOCK_ADD)->UseRealTime()->Unit(benchmark::kMillisecond);