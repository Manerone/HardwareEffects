# Cache False Sharing

In this experiment we observe the phenomena of false sharing of caches. This usually happens when you have two or more threads modifying different elements that happen to be on the same cache line. This will make the separated threads wait on one another, and can have devastating results on performance.

## Requirements

CMake and Google Benchmark installed in the system. I tried to use fetch content to install Google Benchmark only within the project, but failed to do so.

## How to compile

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## How to execute the benchmarks

```bash
./build/false_sharing
```

## References

If you somehow ended up here, be sure to check the links below.

Thanks to this [blog post](https://coffeebeforearch.github.io/2019/12/28/false-sharing-tutorial.html) for a nice tutorial about the subject. It was enough to use it as a basis for my own experiments. 

Thanks to the talk given by Fedor Pikus on CppCon 2017 ([link](https://youtu.be/ZQFzMfHIxng?si=n2j7P782lwJ9jimn)), which sparked my curiosity about the subject.
