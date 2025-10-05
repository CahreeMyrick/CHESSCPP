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
