#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <cmath>
#include <optional>
#include <memory>
#include <cassert>

const int ROWS = 8;
const int COLS = 8;

class Board; // forward declare so we can reference it in Piece

// ==================== Piece base ====================
class Piece {
public:
    std::string color; // "white", "black", or "none" for empty
    bool hasMoved = false;       // for castling & pawn double-steps
    Piece(std::string col = "none") : color(std::move(col)) {}
    virtual ~Piece() = default;
    virtual std::string display() { return "?"; }

    // Pseudo-legal: does not check for self-check, etc.
    virtual bool can_move(const Board &b, int r0, int c0, int r1, int c1) const = 0;
};

class Pawn : public Piece {
public:
    explicit Pawn(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "P" : "p"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class Knight : public Piece {
public:
    explicit Knight(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "N" : "n"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class Bishop : public Piece {
public:
    explicit Bishop(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "B" : "b"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class Rook : public Piece {
public:
    explicit Rook(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "R" : "r"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class Queen : public Piece {
public:
    explicit Queen(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "Q" : "q"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class King : public Piece {
public:
    explicit King(const std::string &col) : Piece(col) {}
    std::string display() override { return (color == "white") ? "K" : "k"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
};

class Empty_Square : public Piece {
public:
    Empty_Square() : Piece("none") {}
    std::string display() override { return "-"; }
    bool can_move(const Board&, int, int, int, int) const override { return false; }
};

// ==================== Board ====================
class Board {
public:
    Piece* board[ROWS][COLS]{}; // nullptr-initialized

    void set_major_pieces(const std::string& color, int row) {
        // rooks
        board[row][0] = new Rook(color);
        board[row][7] = new Rook(color);
        // knights
        board[row][1] = new Knight(color);
        board[row][6] = new Knight(color);
        // bishops
        board[row][2] = new Bishop(color);
        board[row][5] = new Bishop(color);
        // queen & king
        board[row][3] = new Queen(color);
        board[row][4] = new King(color);

        // pawns
        int pawn_row = (row == 0) ? 1 : 6;
        for (int j = 0; j < COLS; j++) {
            board[pawn_row][j] = new Pawn(color);
        }
    }

    void create_board() {
        // Place pieces (board is nullptr-initialized)
        set_major_pieces("white", 0);
        set_major_pieces("black", 7);

        // Fill empty ranks (2..5) with Empty_Square objects
        for (int i = 2; i < 6; ++i) {
            for (int j = 0; j < COLS; ++j) {
                board[i][j] = new Empty_Square();
            }
        }
    }

    void display_board() const {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (board[i][j]) std::cout << board[i][j]->display() << " ";
                else std::cout << "- ";
            }
            std::cout << std::endl;
        }
    }

    bool in_bounds(int r, int c) const {
        return r >= 0 && r < ROWS && c >= 0 && c < COLS;
    }

    bool is_empty(int r, int c) const {
        Piece* p = board[r][c];
        return p == nullptr || dynamic_cast<Empty_Square*>(p) != nullptr || p->color == "none";
    }

    bool is_friend(int r, int c, const std::string& col) const {
        return !is_empty(r, c) && board[r][c]->color == col;
    }

    bool is_enemy(int r, int c, const std::string& col) const {
        return !is_empty(r, c) && board[r][c]->color != col;
    }

    // For sliders (rook/bishop/queen): check that squares BETWEEN are empty.
    bool path_clear(int r0, int c0, int r1, int c1) const {
        int dr = (r1 > r0) - (r1 < r0);  // -1,0,1
        int dc = (c1 > c0) - (c1 < c0);  // -1,0,1
        if (dr == 0 && dc == 0) return true;    // same square (callers should prevent)
        int r = r0 + dr, c = c0 + dc;
        while (r != r1 || c != c1) {
            if (!in_bounds(r, c) || !is_empty(r, c)) return false;
            r += dr; c += dc;
        }
        return true;
    }

    // Locate a king by color
    std::pair<int,int> king_pos(const std::string& col) const {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (board[r][c] && board[r][c]->color == col && dynamic_cast<King*>(board[r][c]))
                    return {r,c};
        return {-1,-1}; // not found (shouldn't happen mid-game)
    }

    // Is square (r,c) attacked by any piece of 'attackerColor'?
    bool attacks_square(const std::string& attackerColor, int r, int c) const {
        auto inb = [&](int rr, int cc){ return rr>=0 && rr<ROWS && cc>=0 && cc<COLS; };

        // Pawn directions
        auto pawn_dir = [&](const std::string& col){ return (col=="white") ? +1 : -1; };

        for (int rr=0; rr<ROWS; ++rr) {
            for (int cc=0; cc<COLS; ++cc) {
                Piece* p = board[rr][cc];
                if (!p || p->color != attackerColor) continue;

                // Knight
                if (dynamic_cast<Knight*>(p)) {
                    int dr = std::abs(r-rr), dc = std::abs(c-cc);
                    if ((dr==2 && dc==1) || (dr==1 && dc==2)) return true;
                    continue;
                }
                // King
                if (dynamic_cast<King*>(p)) {
                    int dr = std::abs(r-rr), dc = std::abs(c-cc);
                    if (std::max(dr,dc)==1) return true;
                    continue;
                }
                // Pawn (captures only diagonally)
                if (dynamic_cast<Pawn*>(p)) {
                    int dir = pawn_dir(p->color);
                    if (r == rr + dir && std::abs(c - cc) == 1) return true;
                    continue;
                }
                // Bishop / Rook / Queen sliding
                auto ray = [&](int drr, int dcc)->bool {
                    int tr = rr + drr, tc = cc + dcc;
                    while (inb(tr,tc)) {
                        if (tr == r && tc == c) return true;
                        if (!is_empty(tr,tc)) break;
                        tr += drr; tc += dcc;
                    }
                    return false;
                };
                if (dynamic_cast<Bishop*>(p) || dynamic_cast<Queen*>(p)) {
                    if (ray(+1,+1) || ray(+1,-1) || ray(-1,+1) || ray(-1,-1)) return true;
                }
                if (dynamic_cast<Rook*>(p) || dynamic_cast<Queen*>(p)) {
                    if (ray(+1,0) || ray(-1,0) || ray(0,+1) || ray(0,-1)) return true;
                }
            }
        }
        return false;
    }

    ~Board() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                delete board[i][j]; // delete nullptr is OK
            }
        }
    }
};

// ==================== Movement logic (pseudo-legal) ====================
bool Pawn::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;

    int dir = (color == "white") ? +1 : -1;     // white moves "down" (increasing row)
    int start_row = (color == "white") ? 1 : 6; // setup: white pawns on row 1

    int dr = r1 - r0;
    int dc = c1 - c0;

    // Forward 1
    if (dc == 0 && dr == dir && b.is_empty(r1, c1)) return true;

    // Forward 2 from start (both squares must be empty)
    if (dc == 0 && dr == 2*dir && r0 == start_row) {
        int midr = r0 + dir;
        if (b.is_empty(midr, c0) && b.is_empty(r1, c1)) return true;
    }

    // Diagonal capture
    if (std::abs(dc) == 1 && dr == dir && b.is_enemy(r1, c1, color)) return true;

    // En passant handled at Game level (destination is empty there)
    return false;
}

bool Knight::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
}

bool Bishop::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    if (dr != dc) return false;
    return b.path_clear(r0, c0, r1, c1);
}

bool Rook::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;
    if (r0 != r1 && c0 != c1) return false;
    return b.path_clear(r0, c0, r1, c1);
}

bool Queen::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    if (!(r0 == r1 || c0 == c1 || dr == dc)) return false;
    return b.path_clear(r0, c0, r1, c1);
}

bool King::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    // Normal king move (castling handled by Game)
    return std::max(dr, dc) == 1 || (dr==0 && std::abs(c1-c0)==2);
    // The two-square horizontal is a castling *attempt*; full checks in Game.
}

// ==================== Game ====================
class Game {
    Board b;
    std::string turn = "white";
    struct EP {
        bool valid = false;
        int target_r = -1, target_c = -1;  // square the capturing pawn moves TO
        int captured_r = -1, captured_c = -1; // square of pawn to be removed
        std::string pawnColor; // color of pawn that double-moved
    } ep;

    static int c2i(char c) {
        if (c < '0' || c > '7') throw std::out_of_range("index not 0-7");
        return c - '0';
    }
    static std::string other(const std::string& col) {
        return (col == "white") ? "black" : "white";
    }

    bool in_check(const std::string& col) {
        auto [kr,kc] = b.king_pos(col);
        if (kr < 0) return false; // should not happen
        return b.attacks_square(other(col), kr, kc);
    }

    // Generic "would this move leave my king in check?" (supports optional extra capture square for EP)
    bool leaves_self_in_check(int r0,int c0,int r1,int c1, std::optional<std::pair<int,int>> extra_capture = std::nullopt) {
        Piece* src = b.board[r0][c0];
        Piece* dst = b.board[r1][c1];
        if (!src) return true;

        // Save extra captured
        Piece* extra = nullptr;
        int er=-1, ec=-1;
        if (extra_capture) {
            er = extra_capture->first;
            ec = extra_capture->second;
            extra = b.board[er][ec];
        }

        // Simulate
        b.board[r1][c1] = src;
        b.board[r0][c0] = nullptr;
        if (extra_capture) b.board[er][ec] = nullptr;

        bool check = in_check(src->color);

        // Revert
        b.board[r0][c0] = src;
        b.board[r1][c1] = dst;
        if (extra_capture) b.board[er][ec] = extra;

        return check;
    }

    bool can_castle_king_side(const std::string& col) {
        int row = (col=="white") ? 0 : 7;
        int kcol = 4, rcol = 7;
        Piece* king = b.board[row][kcol];
        Piece* rook = b.board[row][rcol];
        if (!king || !rook) return false;
        if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook)) return false;
        if (king->color!=col || rook->color!=col) return false;
        if (king->hasMoved || rook->hasMoved) return false;
        if (!b.path_clear(row, kcol, row, rcol)) return false;
        // King may not be in check, and the two traversed squares must not be attacked
        if (in_check(col)) return false;
        if (b.attacks_square(other(col), row, kcol+1)) return false;
        if (b.attacks_square(other(col), row, kcol+2)) return false;
        return true;
    }

    bool can_castle_queen_side(const std::string& col) {
        int row = (col=="white") ? 0 : 7;
        int kcol = 4, rcol = 0;
        Piece* king = b.board[row][kcol];
        Piece* rook = b.board[row][rcol];
        if (!king || !rook) return false;
        if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook)) return false;
        if (king->color!=col || rook->color!=col) return false;
        if (king->hasMoved || rook->hasMoved) return false;
        if (!b.path_clear(row, kcol, row, rcol)) return false; // clears b/w k..r
        if (in_check(col)) return false;
        if (b.attacks_square(other(col), row, kcol-1)) return false;
        if (b.attacks_square(other(col), row, kcol-2)) return false;
        return true;
    }

    void do_castle_king_side(const std::string& col) {
        int row = (col=="white") ? 0 : 7;
        // Move king e->g (4->6), rook h->f (7->5)
        Piece*& king = b.board[row][4];
        Piece*& rook = b.board[row][7];
        Piece*& g = b.board[row][6];
        Piece*& f = b.board[row][5];
        delete g; delete f; // delete any empties
        g = king; f = rook;
        king = new Empty_Square();
        rook = new Empty_Square();
        g->hasMoved = true;
        f->hasMoved = true;
    }

    void do_castle_queen_side(const std::string& col) {
        int row = (col=="white") ? 0 : 7;
        // Move king e->c (4->2), rook a->d (0->3)
        Piece*& king = b.board[row][4];
        Piece*& rook = b.board[row][0];
        Piece*& c = b.board[row][2];
        Piece*& d = b.board[row][3];
        delete c; delete d;
        c = king; d = rook;
        king = new Empty_Square();
        rook = new Empty_Square();
        c->hasMoved = true;
        d->hasMoved = true;
    }

    void maybe_promote(int r1, int c1) {
        Piece* p = b.board[r1][c1];
        if (!p) return;
        Pawn* pawn = dynamic_cast<Pawn*>(p);
        if (!pawn) return;
        if ((p->color=="white" && r1==7) || (p->color=="black" && r1==0)) {
            // auto promote to Queen (simple)
            delete b.board[r1][c1];
            b.board[r1][c1] = new Queen(p->color);
            b.board[r1][c1]->hasMoved = true;
        }
    }

    bool has_any_legal_move(const std::string& col) {
        for (int r0=0;r0<ROWS;++r0)
            for (int c0=0;c0<COLS;++c0) {
                Piece* p = b.board[r0][c0];
                if (!p || p->color!=col) continue;

                // Try all destinations
                for (int r1=0;r1<ROWS;++r1)
                    for (int c1=0;c1<COLS;++c1) {
                        if (r0==r1 && c0==c1) continue;

                        // Special: castling
                        if (dynamic_cast<King*>(p) && r0==r1 && std::abs(c1-c0)==2) {
                            if (c1>c0 ? can_castle_king_side(col) : can_castle_queen_side(col))
                                return true;
                            continue;
                        }

                        // En passant possibility
                        if (dynamic_cast<Pawn*>(p) && std::abs(c1-c0)==1) {
                            int dir = (col=="white")?+1:-1;
                            if (r1==r0+dir && b.is_empty(r1,c1) && ep.valid
                                && ep.target_r==r1 && ep.target_c==c1 && ep.pawnColor!=col) {
                                if (!leaves_self_in_check(r0,c0,r1,c1, std::make_pair(ep.captured_r,ep.captured_c)))
                                    return true;
                                continue;
                            }
                        }

                        // Normal pseudo-legal then legality check
                        if (p->can_move(b,r0,c0,r1,c1)) {
                            if (!leaves_self_in_check(r0,c0,r1,c1))
                                return true;
                        }
                    }
            }
        return false;
    }

public:
    Game() { b.create_board(); }

    void print() const { b.display_board(); }

    // Accepts "P10 30" or "10 30"
    bool parse_move(const std::string& line, int& r0,int& c0,int& r1,int& c1, char& pieceLetter) {
        std::stringstream ss(line);
        std::string a,bm;
        if (!(ss>>a>>bm)) return false;
        int ooff = (std::isdigit(static_cast<unsigned char>(a[0])) ? 0 : 1);
        if ((int)a.size() < ooff+2 || (int)bm.size()!=2) return false;
        pieceLetter = (ooff==0 ? '?' : a[0]);
        r0 = c2i(a[ooff]); c0 = c2i(a[ooff+1]);
        r1 = c2i(bm[0]);   c1 = c2i(bm[1]);
        return true;
    }

    // Make a full legal move; returns false if illegal
    bool move(const std::string& input, std::string& errmsg) {
        int r0,c0,r1,c1; char letter='?';
        if (!parse_move(input, r0,c0,r1,c1, letter)) {
            errmsg = "Format error. Use P10 30 or 10 30";
            return false;
        }

        if (!b.in_bounds(r0,c0) || !b.in_bounds(r1,c1)) {
            errmsg = "Out of bounds";
            return false;
        }

        Piece*& src = b.board[r0][c0];
        if (!src || dynamic_cast<Empty_Square*>(src)) {
            errmsg = "No piece at origin";
            return false;
        }
        if (src->color != turn) {
            errmsg = "It's " + turn + "'s turn";
            return false;
        }

        // Optional sanity: if a letter was supplied, check it matches
        if (letter!='?') {
            char disp = src->display()[0];
            if (std::tolower(static_cast<unsigned char>(disp)) != std::tolower(static_cast<unsigned char>(letter))) {
                errmsg = "Piece letter doesn't match the origin square";
                return false;
            }
        }

        // ---------- Castling ----------
        if (dynamic_cast<King*>(src) && r0==r1 && std::abs(c1-c0)==2) {
            bool kingside = (c1>c0);
            if (kingside ? can_castle_king_side(turn) : can_castle_queen_side(turn)) {
                if (kingside) do_castle_king_side(turn);
                else do_castle_queen_side(turn);
                ep.valid = false; // EP cleared on any non-double-pawn move
                turn = other(turn);
                return true;
            } else {
                errmsg = "Castling not allowed now";
                return false;
            }
        }

        // ---------- En passant ----------
        if (dynamic_cast<Pawn*>(src) && std::abs(c1-c0)==1) {
            int dir = (turn=="white")?+1:-1;
            if (r1==r0+dir && b.is_empty(r1,c1) && ep.valid
                && ep.target_r==r1 && ep.target_c==c1 && ep.pawnColor!=turn) {
                // simulate to check king safety
                if (leaves_self_in_check(r0,c0,r1,c1, std::make_pair(ep.captured_r,ep.captured_c))) {
                    errmsg = "Move would leave king in check";
                    return false;
                }
                // perform EP capture
                delete b.board[ep.captured_r][ep.captured_c];
                b.board[ep.captured_r][ep.captured_c] = new Empty_Square();

                delete b.board[r1][c1]; // should be empty square
                b.board[r1][c1] = src;
                b.board[r0][c0] = new Empty_Square();
                b.board[r1][c1]->hasMoved = true;

                maybe_promote(r1,c1);
                ep.valid = false;
                turn = other(turn);
                return true;
            }
        }

        // ---------- Normal move (pseudo-legal + king safety) ----------
        if (!src->can_move(b,r0,c0,r1,c1)) {
            errmsg = "Illegal move for that piece";
            return false;
        }
        if (leaves_self_in_check(r0,c0,r1,c1)) {
            errmsg = "Move would leave king in check";
            return false;
        }

        // Execute normal move
        delete b.board[r1][c1];
        b.board[r1][c1] = src;
        b.board[r0][c0] = new Empty_Square();
        b.board[r1][c1]->hasMoved = true;

        // EP bookkeeping
        ep.valid = false;
        if (dynamic_cast<Pawn*>(b.board[r1][c1])) {
            int dir = (turn=="white")?+1:-1;
            if (std::abs(r1 - r0) == 2) {
                ep.valid = true;
                ep.target_r = r0 + dir;
                ep.target_c = c0;
                ep.captured_r = r1;
                ep.captured_c = c0;
                ep.pawnColor = turn;
            }
        }

        // Promotion
        maybe_promote(r1,c1);

        // Switch sides
        turn = other(turn);
        return true;
    }

    bool is_checkmate(const std::string& col) {
        return in_check(col) && !has_any_legal_move(col);
    }
    bool is_stalemate(const std::string& col) {
        return !in_check(col) && !has_any_legal_move(col);
    }

    void loop() {
        while (true) {
            std::cout << "\n" << turn << " to move. Enter (e.g.) P10 30 or 10 30. Type 'quit' to exit.\n";
            b.display_board();
            std::cout << "> ";
            std::string line;
            if (!std::getline(std::cin, line)) break;
            if (line=="quit" || line=="exit") break;
            if (line.empty()) continue;

            std::string err;
            if (!move(line, err)) {
                std::cout << "Invalid: " << err << "\n";
                continue;
            }

            if (is_checkmate(turn)) {
                b.display_board();
                std::cout << "Checkmate! " << other(turn) << " wins.\n";
                break;
            }
            if (is_stalemate(turn)) {
                b.display_board();
                std::cout << "Stalemate! Draw.\n";
                break;
            }
            if (in_check(turn)) {
                std::cout << "Check on " << turn << "!\n";
            }
        }
    }
};

// ==================== main ====================
int main() {
    Game game;
    game.loop();
    return 0;
}

