# Atomic Functions

Evaluates how atomic operations behave in CPP, this is inspired by the talk given by Fedor Pikus on CppCon 2017 ([link](https://youtu.be/ZQFzMfHIxng?si=n2j7P782lwJ9jimn)).

Comparison between counting until 1.000.000 using different locking methods: atomic ++, compare and swap, fetch_add, and a lock_guard.

-----------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
-----------------------------------------------------------------------
BM_ATOMIC_ADD/real_time            3.79 ms         3.79 ms          186
BM_CAS_ADD_WEAK/real_time          7.63 ms         7.59 ms           93
BM_CAS_ADD_STRONG/real_time        7.92 ms         7.77 ms           93
BM_FETCH_ADD/real_time             3.77 ms         3.77 ms          183
BM_LOCK_ADD/real_time              5.54 ms         5.53 ms          129

This test was executed on a MacBook Air, M2, 8GB memory.