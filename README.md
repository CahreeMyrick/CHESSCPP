# Chess Engine (UCI)

A small C++ chess engine with:
- a playable game model (`Game`, `Board`, `Piece`, …)
- a simple minimax search (`MinimaxStrategy`)
- a UCI front-end (`uci_main.cpp`)
- tests (`test_chess.cpp`)

Both `uci_main.cpp` and `test_chess.cpp` `#define CHESS_NO_MAIN` before including `minimax.cpp`, so each can compile as its own executable without duplicate `main()` conflicts.

---

## Project Layout
```
.
├── minimax.cpp        # engine: Board/Game/MinimaxStrategy, etc.
├── uci_main.cpp       # UCI loop -> uses Game + MinimaxStrategy
└── test_chess.cpp     # assertions for setup, EP, castling, copy semantics
```
---

## Build

```bash
# CLI engine
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -o minimax minimax.cpp


# Tests
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -o tests test_chess.cpp

# UCI engine
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -o myengine uci_main.cpp
# (Use g++ instead of clang++ if you prefer)
```

### Windows (MinGW)
```bat
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -o tests.exe test_chess.cpp
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -o myengine.exe uci_main.cpp
```


## Run the Tests
Covers initial setup, turn handling, illegal move rejection, en passant, castling, deep-copy behavior, and that legal moves are non-empty at start.

```bash
./tests
# Running tests...
# All tests passed!
```

---

## Run the Engine (UCI)

Talk to the engine from a terminal or plug into a UCI GUI.

```bash
./myengine
```

Example session:
```
uci
id name MyEngine
id author You
uciok
isready
readyok
ucinewgame
position startpos moves e2e4 e7e5 g1f3
go depth 3
bestmove g1f3
```

---

## Move Formats

- **UCI (external):** `e2e4`, `e7e8q`, etc.
- **Engine-internal (used by `Game::move`)**: two digit pairs `"rc rc"` where `r,c ∈ {0..7}`.  
  Example: `"14 34"` means `(r=1,c=4) → (r=3,c=4)` i.e., `e2 → e4`.

Conversion helpers (in `uci_main.cpp`):
- `uci_move_to_engine("e2e4")  -> "14 34"`
- `engine_move_to_uci("14 34") -> "e2e4"`

---

## Baseline & Benchmarks (2025-10-10)

**Environment**
- Commit: e69960a
- Compiler: g++ 13 (libc++)
- Flags: `-O3 -DNDEBUG -march=native -flto -Wall -Wextra -pedantic`
- Machine: Apple M3 (8 cores / 8 threads), 16 GB RAM, macOS 15.3.1
- Threads: 1 (single-threaded), Transposition Table: none

**Correctness (Perft)**
- **startpos:** d1 20, d2 400, d3 8,902, d4 197,281
- **kiwipete:** d1 48, d2 2,039, d3 97,862, d4 4,085,603

**Performance (1 thread)**
- **startpos**
  - d1 **0.126 ms** (158,572 nps)
  - d2 **2.496 ms** (160,248 nps)
  - d3 **51.893 ms** (171,545 nps)
  - d4 **1,229.99 ms** (160,392 nps)
- **kiwipete**
  - d1 **0.267 ms** (179,999 nps)
  - d2 **11.909 ms** (171,220 nps)
  - d3 **489.386 ms** (199,968 nps)
  - d4 **23,289.4 ms** (175,427 nps)

**Reproduce**
```bash
# Build perft tool
g++ -std=c++20 -O3 -DNDEBUG -march=native -flto -Wall -Wextra -pedantic -o perft perft.cpp
./perft   # exits non-zero if perft counts mismatch known values
