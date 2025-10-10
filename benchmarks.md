# Chess Engine Baseline (2025-10-10)

## Environment
Commit: e69960a
Compiler: g++ 13 (libc++), flags: -O3 -DNDEBUG -march=native -flto
Machine: CPU Model: Apple M3, CPU Cores/Threads: 8 cores/ 8 threads , RAM: 16 GB, macOS Version 15.3.1
Threads: 1, TT: none

## Correctness (Perft)
startpos: d1 20, d2 400, d3 8,902, d4 197,281
kiwipete: d1 48, d2 2,039, d3 97,862, d4 4,085,603

## Performance (this machine)
startpos: 
  d1 0.126 ms (158,572 nps)
  d2 2.496 ms (160,248 nps)
  d3 51.893 ms (171,545 nps)
  d4 1,229.99 ms (160,392 nps)
kiwipete:
  d1 0.267 ms (179,999 nps)
  d2 11.909 ms (171,220 nps)
  d3 489.386 ms (199,968 nps)
  d4 23,289.4 ms (175,427 nps)
