/// In this file we test if atomic variables have to wait each other
/// This is a behavior know as cache false sharing.
/// Thanks to https://coffeebeforearch.github.io/2019/12/28/false-sharing-tutorial.html
/// for a nice explanation of this.

#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <mutex>

void work(std::atomic<int>& a) {
  for(int i = 0; i < 100000; ++i) {
    a++;
  }
}

static void BM_FALSE_SHARING(benchmark::State& state) {
    while(state.KeepRunning()){
        std::atomic<int> x1{0};
        std::atomic<int> x2{0};
        std::atomic<int> x3{0};
        std::atomic<int> x4{0};

        std::thread t0(work, std::ref(x1));
        std::thread t1(work, std::ref(x2));
        std::thread t2(work, std::ref(x3));
        std::thread t3(work, std::ref(x4));
        t0.join();
        t1.join();
        t2.join();
        t3.join();
    }
}

static void BM_FALSE_SHARING_NO_ATOMIC(benchmark::State& state) {

    auto f = [](int &a){
        for(int i = 0; i < 100000; ++i) {
            a++;
        }
    };

    struct Foo {
        int x1;
        int x2;
        int x3;
        int x4;
    };

    while(state.KeepRunning()){
        Foo x;

        std::thread t0(f, std::ref(x.x1));
        std::thread t1(f, std::ref(x.x2));
        std::thread t2(f, std::ref(x.x3));
        std::thread t3(f, std::ref(x.x4));
        t0.join();
        t1.join();
        t2.join();
        t3.join();
    }
}

static void BM_FALSE_SHARING_NO_ATOMIC_LOCAL_VARS(benchmark::State& state) {

    auto f = [](int &a){
        int x = 0;
        for(int i = 0; i < 100000; ++i) {
            x++;
        }
        a = x;
    };

    struct Foo {
        int x1;
        int x2;
        int x3;
        int x4;
    };

    while(state.KeepRunning()){
        Foo x;

        std::thread t0(f, std::ref(x.x1));
        std::thread t1(f, std::ref(x.x2));
        std::thread t2(f, std::ref(x.x3));
        std::thread t3(f, std::ref(x.x4));
        t0.join();
        t1.join();
        t2.join();
        t3.join();
    }
}

static void BM_LOCAL_VARIABLES(benchmark::State& state) {
    auto f = [](){
        std::atomic<int> x{0};
        work(x);
    };
    while(state.KeepRunning()){
        std::thread t0(f);
        std::thread t1(f);
        std::thread t2(f);
        std::thread t3(f);
        t0.join();
        t1.join();
        t2.join();
        t3.join();
    }
}

static void BM_NO_SHARING(benchmark::State& state) {
    struct alignas(64) AlignedType {
    AlignedType() { val = 0; }
    std::atomic<int> val;
    };
    while(state.KeepRunning()){
        AlignedType x1;
        AlignedType x2;
        AlignedType x3;
        AlignedType x4;

        std::thread t0(work, std::ref(x1.val));
        std::thread t1(work, std::ref(x2.val));
        std::thread t2(work, std::ref(x3.val));
        std::thread t3(work, std::ref(x4.val));
        t0.join();
        t1.join();
        t2.join();
        t3.join();
    }
}



BENCHMARK(BM_FALSE_SHARING)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_FALSE_SHARING_NO_ATOMIC)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_FALSE_SHARING_NO_ATOMIC_LOCAL_VARS)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LOCAL_VARIABLES)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_NO_SHARING)->UseRealTime()->Unit(benchmark::kMillisecond);