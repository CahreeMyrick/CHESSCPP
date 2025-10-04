// uci_main.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

#define CHESS_NO_MAIN
#include "minimax.cpp"   // includes your Game/Board/MinimaxStrategy, etc.

// ----- helpers: UCI <-> your engine’s (r,c) format -----
static inline int file_to_col(char f){ return int(f - 'a'); }          // a..h -> 0..7
static inline int rank_to_row(char r){ return int(r - '1'); }          // '1'..'8' -> 0..7
static inline char col_to_file(int c){ return char('a' + c); }
static inline char row_to_rank(int r){ return char('1' + r); }

static std::string uci_move_to_engine(const std::string& u) {
    // u like "e2e4" or "e7e8q" (promotion char optional). We ignore the promo letter since engine auto-queens.
    if (u.size() < 4) return "";
    int c0 = file_to_col(u[0]);
    int r0 = rank_to_row(u[1]);
    int c1 = file_to_col(u[2]);
    int r1 = rank_to_row(u[3]);
    if (r0<0||r0>7||c0<0||c0>7||r1<0||r1>7||c1<0||c1>7) return "";
    std::string a, b;
    a.push_back(char('0'+r0)); a.push_back(char('0'+c0));
    b.push_back(char('0'+r1)); b.push_back(char('0'+c1));
    return a + " " + b;
}

static std::string engine_move_to_uci(const std::string& m) {
    // your moves look like "r0c0 r1c1" but actually "rc rc" with digits, e.g., "14 34"
    if (m.size() != 5 || m[2] != ' ') return "";
    int r0 = m[0]-'0', c0 = m[1]-'0', r1 = m[3]-'0', c1 = m[4]-'0';
    std::string u;
    u += col_to_file(c0); u += row_to_rank(r0);
    u += col_to_file(c1); u += row_to_rank(r1);
    return u; // promotions: engine auto-queens, so no suffix needed
}

// ----- simple engine wrapper -----
struct UciEngine {
    Game game;
    MinimaxStrategy strat;
    bool thinking = false;

    UciEngine() { strat.max_depth = 3; }

    void new_game() { game = Game{}; }

    // position startpos [moves ...]   (FEN support can be added later)
    void set_position_from_cmd(const std::string& cmd) {
        std::istringstream ss(cmd);
        std::string tok; ss >> tok;        // "position"
        ss >> tok;                          // "startpos" | "fen"
        if (tok == "startpos") {
            game = Game{};
            if (ss >> tok && tok == "moves") {
                std::string um;
                while (ss >> um) {
                    std::string mv = uci_move_to_engine(um);
                    std::string err;
                    if (!game.move(mv, err)) {
                        // ignore bad input from GUI (rare)
                    }
                }
            }
        } else if (tok == "fen") {
            // (optional) parse FEN here if you add FEN->Board support
            // For now, ignore; most GUIs use startpos+moves.
        }
    }

    // go depth N | go movetime T(ms)
    void go(const std::string& cmd) {
        thinking = true;

        int depth = strat.max_depth;        // default
        int movetime_ms = -1;

        // parse args
        {
            std::istringstream ss(cmd);
            std::string tok; ss >> tok; // "go"
            while (ss >> tok) {
                if (tok == "depth") { ss >> depth; }
                else if (tok == "movetime") { ss >> movetime_ms; }
                // (You can parse wtime/btime/inc for time mgmt later)
            }
        }

        // set depth (we ignore movetime for now; add time cutoff later)
        strat.max_depth = std::max(1, depth);

        // search
        std::string best = strat.select_move(game);

        if (best.empty()) {
            std::cout << "bestmove 0000\n";
        } else {
            std::cout << "bestmove " << engine_move_to_uci(best) << "\n";
        }
        std::cout.flush();
        thinking = false;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.setf(std::ios::unitbuf); // auto-flush

    UciEngine E;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name MyEngine\n";
            std::cout << "id author You\n";
            std::cout << "uciok\n";
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line.rfind("setoption", 0) == 0) {
            // ignore for now or parse options
        } else if (line == "ucinewgame") {
            E.new_game();
        } else if (line.rfind("position", 0) == 0) {
            E.set_position_from_cmd(line);
        } else if (line.rfind("go", 0) == 0) {
            E.go(line);
        } else if (line == "stop") {
            // No async search here; nothing to cancel.
        } else if (line == "quit") {
            break;
        }
    }
    return 0;
}

