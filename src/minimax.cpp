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

constexpr int ROWS = 8;
constexpr int COLS = 8;

class Board; // forward declare

// -------------------- Color helpers --------------------
enum class Color { White, Black, None };

inline const char* to_cstr(Color c) {
    switch (c) {
        case Color::White: return "white";
        case Color::Black: return "black";
        default:           return "none";
    }
}
inline Color other(Color c) {
    return c == Color::White ? Color::Black :
           c == Color::Black ? Color::White : Color::None;
}

// ==================== Piece base ====================
class Piece {
public:
    Color color = Color::None;     // piece side
    bool hasMoved = false;         // castling & pawn double-steps

    explicit Piece(Color col = Color::None) : color(col) {}
    virtual ~Piece() = default;

    virtual std::string display() const { return "?"; }

    // Pseudo-legal: does not check for self-check, etc.
    virtual bool can_move(const Board &b, int r0, int c0, int r1, int c1) const = 0;

    // Deep copy (for copying positions)
    virtual std::unique_ptr<Piece> clone() const = 0;
};

class Pawn : public Piece {
public:
    explicit Pawn(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "P" : "p"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Pawn>(*this); }
};

class Knight : public Piece {
public:
    explicit Knight(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "N" : "n"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Knight>(*this); }
};

class Bishop : public Piece {
public:
    explicit Bishop(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "B" : "b"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Bishop>(*this); }
};

class Rook : public Piece {
public:
    explicit Rook(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "R" : "r"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Rook>(*this); }
};

class Queen : public Piece {
public:
    explicit Queen(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "Q" : "q"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Queen>(*this); }
};

class King : public Piece {
public:
    explicit King(Color col) : Piece(col) {}
    std::string display() const override { return (color == Color::White) ? "K" : "k"; }
    bool can_move(const Board& b, int r0, int c0, int r1, int c1) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<King>(*this); }
};

class Empty_Square : public Piece {
public:
    Empty_Square() : Piece(Color::None) {}
    std::string display() const override { return "-"; }
    bool can_move(const Board&, int, int, int, int) const override { return false; }
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Empty_Square>(*this); }
};

// ==================== Board ====================
class Board {
public:
    std::unique_ptr<Piece> board[ROWS][COLS]; // nullptr means empty

    Board() = default;
    Board(const Board& other) {
        for (int r=0;r<ROWS;++r)
            for (int c=0;c<COLS;++c)
                board[r][c] = other.board[r][c] ? other.board[r][c]->clone() : nullptr;
    }
    Board& operator=(const Board& other){
        if (this==&other) return *this;
        for (int r=0;r<ROWS;++r)
            for (int c=0;c<COLS;++c)
                board[r][c] = other.board[r][c] ? other.board[r][c]->clone() : nullptr;
        return *this;
    }

    void set_major_pieces(Color color, int row) {
        // rooks
        board[row][0] = std::make_unique<Rook>(color);
        board[row][7] = std::make_unique<Rook>(color);
        // knights
        board[row][1] = std::make_unique<Knight>(color);
        board[row][6] = std::make_unique<Knight>(color);
        // bishops
        board[row][2] = std::make_unique<Bishop>(color);
        board[row][5] = std::make_unique<Bishop>(color);
        // queen & king
        board[row][3] = std::make_unique<Queen>(color);
        board[row][4] = std::make_unique<King>(color);

        // pawns
        int pawn_row = (row == 0) ? 1 : 6;
        for (int j = 0; j < COLS; j++) {
            board[pawn_row][j] = std::make_unique<Pawn>(color);
        }
    }

    void create_board() {
        // Place pieces (leave others as nullptr = empty)
        set_major_pieces(Color::White, 0);
        set_major_pieces(Color::Black, 7);
        // Middle ranks remain nullptr (treated as empty).
    }

    void display_board() const {
        auto colLetters = []() {
            std::cout << "    ";
            for (int c = 0; c < COLS; ++c) std::cout << "  " << char('a' + c) << " ";
            std::cout << "\n";
        };

        const char* TL = "┌"; const char* TR = "┐";
        const char* BL = "└"; const char* BR = "┘";
        const char* T  = "┬"; const char* M  = "┼"; const char* B  = "┴";
        const char* H  = "───"; const char* V = "│";

        auto top_border = [&]() {
            std::cout << "    " << TL;
            for (int c = 0; c < COLS; ++c) {
                std::cout << H << (c == COLS - 1 ? TR : T);
            }
            std::cout << "\n";
        };
        auto mid_border = [&]() {
            std::cout << "    " << "├";
            for (int c = 0; c < COLS; ++c) {
                std::cout << H << (c == COLS - 1 ? "┤" : M);
            }
            std::cout << "\n";
        };
        auto bot_border = [&]() {
            std::cout << "    " << BL;
            for (int c = 0; c < COLS; ++c) {
                std::cout << H << (c == COLS - 1 ? BR : B);
            }
            std::cout << "\n";
        };

        colLetters();
        top_border();

        for (int r = 0; r < ROWS; ++r) {
            std::cout << "  " << r << " " << V;
            for (int c = 0; c < COLS; ++c) {
                char pc = board[r][c] ? board[r][c]->display()[0] : '-';
                std::cout << " " << pc << " " << V;
            }
            std::cout << " " << r << "\n";
            if (r != ROWS - 1) mid_border();
        }

        bot_border();
        colLetters();
    }

    bool in_bounds(int r, int c) const {
        return r >= 0 && r < ROWS && c >= 0 && c < COLS;
    }

    bool is_empty(int r, int c) const {
        if (!board[r][c]) return true;
        return (board[r][c]->color == Color::None) ||
               (dynamic_cast<Empty_Square*>(board[r][c].get()) != nullptr);
    }

    bool is_friend(int r, int c, Color col) const {
        return !is_empty(r, c) && board[r][c]->color == col;
    }

    bool is_enemy(int r, int c, Color col) const {
        return !is_empty(r, c) && board[r][c]->color != col;
    }

    // For sliders (rook/bishop/queen): check that squares BETWEEN are empty.
    bool path_clear(int r0, int c0, int r1, int c1) const {
        int dr = (r1 > r0) - (r1 < r0);  // -1,0,1
        int dc = (c1 > c0) - (c1 < c0);  // -1,0,1
        if (dr == 0 && dc == 0) return true;
        int r = r0 + dr, c = c0 + dc;
        while (r != r1 || c != c1) {
            if (!in_bounds(r, c) || !is_empty(r, c)) return false;
            r += dr; c += dc;
        }
        return true;
    }

    // Locate a king by color
    std::pair<int,int> king_pos(Color col) const {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (board[r][c] && board[r][c]->color == col && dynamic_cast<King*>(board[r][c].get()))
                    return {r,c};
        return {-1,-1};
    }

    // Is square (r,c) attacked by any piece of 'attackerColor'?
    bool attacks_square(Color attackerColor, int r, int c) const {
        auto inb = [&](int rr, int cc){ return rr>=0 && rr<ROWS && cc>=0 && cc<COLS; };
        auto pawn_dir = [&](Color col){ return (col==Color::White) ? +1 : -1; };

        for (int rr=0; rr<ROWS; ++rr) {
            for (int cc=0; cc<COLS; ++cc) {
                const Piece* p = board[rr][cc].get();
                if (!p || p->color != attackerColor) continue;

                // Knight
                if (dynamic_cast<const Knight*>(p)) {
                    int dr = std::abs(r-rr), dc = std::abs(c-cc);
                    if ((dr==2 && dc==1) || (dr==1 && dc==2)) return true;
                    continue;
                }
                // King
                if (dynamic_cast<const King*>(p)) {
                    int dr = std::abs(r-rr), dc = std::abs(c-cc);
                    if (std::max(dr,dc)==1) return true;
                    continue;
                }
                // Pawn (captures only diagonally)
                if (dynamic_cast<const Pawn*>(p)) {
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
                if (dynamic_cast<const Bishop*>(p) || dynamic_cast<const Queen*>(p)) {
                    if (ray(+1,+1) || ray(+1,-1) || ray(-1,+1) || ray(-1,-1)) return true;
                }
                if (dynamic_cast<const Rook*>(p) || dynamic_cast<const Queen*>(p)) {
                    if (ray(+1,0) || ray(-1,0) || ray(0,+1) || ray(0,-1)) return true;
                }
            }
        }
        return false;
    }
};

// ==================== Movement logic (pseudo-legal) ====================
bool Pawn::can_move(const Board& b, int r0, int c0, int r1, int c1) const {
    if (r0 == r1 && c0 == c1) return false;
    if (!b.in_bounds(r1, c1) || b.is_friend(r1, c1, color)) return false;

    int dir = (color == Color::White) ? +1 : -1;     // white moves "down" (increasing row)
    int start_row = (color == Color::White) ? 1 : 6; // white pawns start at row 1

    int dr = r1 - r0;
    int dc = c1 - c0;

    // Forward 1
    if (dc == 0 && dr == dir && b.is_empty(r1, c1)) return true;

    // Forward 2 from start
    if (dc == 0 && dr == 2*dir && r0 == start_row) {
        int midr = r0 + dir;
        if (b.is_empty(midr, c0) && b.is_empty(r1, c1)) return true;
    }

    // Diagonal capture
    if (std::abs(dc) == 1 && dr == dir && b.is_enemy(r1, c1, color)) return true;

    // En passant handled at Game level
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
}

// ==================== Game + Minimax ====================
struct Strategy; // fwd

class Game {
    Board b;
    Color turn = Color::White;

    struct EP {
        bool valid = false;
        int target_r = -1, target_c = -1;    // square capturing pawn moves TO
        int captured_r = -1, captured_c = -1;// square of pawn to be removed
        Color pawnColor = Color::None;       // color of pawn that double-moved
    } ep;

    static int c2i(char c) {
        if (c < '0' || c > '7') throw std::out_of_range("index not 0-7");
        return c - '0';
    }

    bool in_check(Color col) const {
        auto [kr,kc] = b.king_pos(col);
        if (kr < 0) return false; // not found
        return b.attacks_square(other(col), kr, kc);
    }

    // Simulate (optionally removing an extra captured piece for EP), then restore.
    bool leaves_self_in_check(int r0,int c0,int r1,int c1,
                          std::optional<std::pair<int,int>> extra_capture = std::nullopt)
    {
        auto& from = b.board[r0][c0];
        auto& to   = b.board[r1][c1];
        if (!from) return true;

        Color mover = from->color;

        // Handle temporary EP removal (captured pawn behind the target square)
        std::unique_ptr<Piece> ep_saved;
        int er=-1, ec=-1;
        if (extra_capture) {
            er = extra_capture->first;
            ec = extra_capture->second;
            ep_saved = std::move(b.board[er][ec]); // temporarily remove captured pawn
        }

        // ---- Proper capture simulation (no swap) ----
        // Move 'from' to 'to', and *remove* whatever was in 'to' during the test.
        std::unique_ptr<Piece> captured = std::move(to);   // may be null
        std::unique_ptr<Piece> moving   = std::move(from); // must exist
        to   = std::move(moving);                          // piece now at destination
        // 'from' is now empty (nullptr)

        bool check = in_check(mover);

        // ---- Revert ----
        from = std::move(to);           // move piece back to origin
        to   = std::move(captured);     // restore captured piece (if any)
        if (extra_capture) {
            b.board[er][ec] = std::move(ep_saved); // restore EP-captured pawn
        }

        return check;
    }

    bool can_castle_king_side(Color col) const {
        int row = (col==Color::White) ? 0 : 7;
        int kcol = 4, rcol = 7;
        const Piece* king = b.board[row][kcol].get();
        const Piece* rook = b.board[row][rcol].get();
        if (!king || !rook) return false;
        if (!dynamic_cast<const King*>(king) || !dynamic_cast<const Rook*>(rook)) return false;
        if (king->color!=col || rook->color!=col) return false;
        if (king->hasMoved || rook->hasMoved) return false;
        if (!b.path_clear(row, kcol, row, rcol)) return false;
        if (in_check(col)) return false;
        if (b.attacks_square(other(col), row, kcol+1)) return false;
        if (b.attacks_square(other(col), row, kcol+2)) return false;
        return true;
    }

    bool can_castle_queen_side(Color col) const {
        int row = (col==Color::White) ? 0 : 7;
        int kcol = 4, rcol = 0;
        const Piece* king = b.board[row][kcol].get();
        const Piece* rook = b.board[row][rcol].get();
        if (!king || !rook) return false;
        if (!dynamic_cast<const King*>(king) || !dynamic_cast<const Rook*>(rook)) return false;
        if (king->color!=col || rook->color!=col) return false;
        if (king->hasMoved || rook->hasMoved) return false;
        if (!b.path_clear(row, kcol, row, rcol)) return false;
        if (in_check(col)) return false;
        if (b.attacks_square(other(col), row, kcol-1)) return false;
        if (b.attacks_square(other(col), row, kcol-2)) return false;
        return true;
    }

    void do_castle_king_side(Color col) {
        int row = (col==Color::White) ? 0 : 7;
        // king e->g (4->6), rook h->f (7->5)
        auto& king = b.board[row][4];
        auto& rook = b.board[row][7];
        b.board[row][6] = std::move(king);
        b.board[row][5] = std::move(rook);
        b.board[row][4].reset();
        b.board[row][7].reset();
        b.board[row][6]->hasMoved = true;
        b.board[row][5]->hasMoved = true;
    }

    void do_castle_queen_side(Color col) {
        int row = (col==Color::White) ? 0 : 7;
        // king e->c (4->2), rook a->d (0->3)
        auto& king = b.board[row][4];
        auto& rook = b.board[row][0];
        b.board[row][2] = std::move(king);
        b.board[row][3] = std::move(rook);
        b.board[row][4].reset();
        b.board[row][0].reset();
        b.board[row][2]->hasMoved = true;
        b.board[row][3]->hasMoved = true;
    }

    void maybe_promote(int r1, int c1) {
        if (!b.board[r1][c1]) return;
        if (dynamic_cast<Pawn*>(b.board[r1][c1].get()) == nullptr) return;
        // white promotes at row 7, black at row 0
        if ((b.board[r1][c1]->color==Color::White && r1==7) ||
            (b.board[r1][c1]->color==Color::Black && r1==0))
        {
            Color col = b.board[r1][c1]->color;
            b.board[r1][c1] = std::make_unique<Queen>(col); // auto-queen
            b.board[r1][c1]->hasMoved = true;
        }
    }

    bool has_any_legal_move(Color col) {
        for (int r0=0;r0<ROWS;++r0)
            for (int c0=0;c0<COLS;++c0) {
                Piece* p = b.board[r0][c0].get();
                if (!p || p->color!=col) continue;

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
                            int dir = (col==Color::White)?+1:-1;
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
    Game(const Game& g) : b(g.b), turn(g.turn), ep(g.ep) {}
    Game& operator=(const Game& g){ b=g.b; turn=g.turn; ep=g.ep; return *this; }

    void print() const { b.display_board(); }

    const Board& get_board() const { return b; }
    Color side_to_move() const { return turn; }

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

        auto& src = b.board[r0][c0];
        if (!src || dynamic_cast<Empty_Square*>(src.get())) {
            errmsg = "No piece at origin";
            return false;
        }
        if (src->color != turn) {
            errmsg = std::string("It's ") + to_cstr(turn) + "'s turn";
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
        if (dynamic_cast<King*>(src.get()) && r0==r1 && std::abs(c1-c0)==2) {
            bool kingside = (c1>c0);
            if (kingside ? can_castle_king_side(turn) : can_castle_queen_side(turn)) {
                if (kingside) do_castle_king_side(turn);
                else          do_castle_queen_side(turn);
                ep.valid = false; // EP cleared on any non-double-pawn move
                turn = other(turn);
                return true;
            } else {
                errmsg = "Castling not allowed now";
                return false;
            }
        }

        // ---------- En passant ----------
        if (dynamic_cast<Pawn*>(src.get()) && std::abs(c1-c0)==1) {
            int dir = (turn==Color::White)?+1:-1;
            if (r1==r0+dir && b.is_empty(r1,c1) && ep.valid
                && ep.target_r==r1 && ep.target_c==c1 && ep.pawnColor!=turn) {

                if (leaves_self_in_check(r0,c0,r1,c1, std::make_pair(ep.captured_r,ep.captured_c))) {
                    errmsg = "Move would leave king in check";
                    return false;
                }
                // perform EP capture
                b.board[ep.captured_r][ep.captured_c].reset();
                b.board[r1][c1] = std::move(b.board[r0][c0]);
                b.board[r0][c0].reset();
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
        b.board[r1][c1].reset(); // drop captured piece if any
        b.board[r1][c1] = std::move(b.board[r0][c0]);
        b.board[r0][c0].reset();
        b.board[r1][c1]->hasMoved = true;

        // EP bookkeeping
        ep.valid = false;
        if (dynamic_cast<Pawn*>(b.board[r1][c1].get())) {
            int dir = (turn==Color::White)?+1:-1;
            if (std::abs(r1 - r0) == 2) {
                ep.valid = true;
                ep.target_r = r0 + dir;
                ep.target_c = c0;
                ep.captured_r = r1;
                ep.captured_c = c0;
                ep.pawnColor  = turn;
            }
        }

        // Promotion
        maybe_promote(r1,c1);

        // Switch sides
        turn = other(turn);
        return true;
    }

    bool is_checkmate(Color col) {
        return in_check(col) && !has_any_legal_move(col);
    }
    bool is_stalemate(Color col) {
        return !in_check(col) && !has_any_legal_move(col);
    }

    // --------- Move generation for search (reuses legality checks) ---------
    std::vector<std::string> legal_moves() {
        std::vector<std::string> out;
        auto fmt = [](int r, int c){
            std::string s; s.push_back(char('0'+r)); s.push_back(char('0'+c)); return s;
        };

        for (int r0=0;r0<ROWS;++r0) for (int c0=0;c0<COLS;++c0) {
            Piece* p = b.board[r0][c0].get();
            if (!p || p->color!=turn) continue;

            for (int r1=0;r1<ROWS;++r1) for (int c1=0;c1<COLS;++c1) {
                if (r0==r1 && c0==c1) continue;

                // Castling
                if (dynamic_cast<King*>(p) && r0==r1 && std::abs(c1-c0)==2) {
                    bool ks = c1>c0;
                    if (ks ? can_castle_king_side(turn) : can_castle_queen_side(turn))
                        out.push_back(fmt(r0,c0) + " " + fmt(r1,c1));
                    continue;
                }

                // En passant (mirror move() logic)
                if (dynamic_cast<Pawn*>(p) && std::abs(c1-c0)==1) {
                    int dir = (turn==Color::White)?+1:-1;
                    if (r1==r0+dir && b.is_empty(r1,c1) && ep.valid
                        && ep.target_r==r1 && ep.target_c==c1 && ep.pawnColor!=turn) {
                        if (!leaves_self_in_check(r0,c0,r1,c1, std::make_pair(ep.captured_r,ep.captured_c)))
                            out.push_back(fmt(r0,c0) + " " + fmt(r1,c1));
                        continue;
                    }
                }

                // Normal moves
                if (p->can_move(b,r0,c0,r1,c1) && !leaves_self_in_check(r0,c0,r1,c1))
                    out.push_back(fmt(r0,c0) + " " + fmt(r1,c1));
            }
        }
        return out;
    }

    // --------- Interactive loops ---------
    void loop() {
        while (true) {
            std::cout << "\n" << to_cstr(turn) << " to move. Enter (e.g.) P10 30 or 10 30. Type 'quit' to exit.\n";
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
                std::cout << "Checkmate! " << to_cstr(other(turn)) << " wins.\n";
                break;
            }
            if (is_stalemate(turn)) {
                b.display_board();
                std::cout << "Stalemate! Draw.\n";
                break;
            }
            if (in_check(turn)) {
                std::cout << "Check on " << to_cstr(turn) << "!\n";
            }
        }
    }

    void loop_with_strategies(Strategy* white, Strategy* black);
};

// ==================== Evaluator ====================
int evaluate(const Game& g) {
    const Board& b = g.get_board();

    auto val = [&](const Piece* p)->int {
        if (!p) return 0;
        int s = (p->color==Color::White) ? +1 : -1;
        if (dynamic_cast<const Pawn*>(p))   return 100*s;
        if (dynamic_cast<const Knight*>(p)) return 320*s;
        if (dynamic_cast<const Bishop*>(p)) return 330*s;
        if (dynamic_cast<const Rook*>(p))   return 500*s;
        if (dynamic_cast<const Queen*>(p))  return 900*s;
        return 0; // King not scored here
    };

    int score = 0;
    for (int r=0;r<ROWS;++r)
        for (int c=0;c<COLS;++c)
            score += val(b.board[r][c].get());

    // Tiny mobility bonus for side to move
    Game tmp = g;
    int my_moves = (int)tmp.legal_moves().size();
    score += (tmp.side_to_move()==Color::White ? +my_moves : -my_moves);

    return score; // positive = good for White
}

// ==================== Strategy + Minimax ====================
struct Strategy {
    virtual ~Strategy() = default;
    virtual std::string select_move(const Game& g) = 0;
};

struct MinimaxStrategy : Strategy {
    int max_depth = 3; // start with 2–3

    int search(Game& pos, int depth, int alpha, int beta) {
        if (depth==0) return evaluate(pos);

        auto moves = pos.legal_moves();
        if (moves.empty()) {
            if (pos.is_checkmate(pos.side_to_move()))
                return (pos.side_to_move()==Color::White ? -100000 : +100000);
            return 0; // stalemate
        }

        bool maxing = (pos.side_to_move()==Color::White);
        if (maxing) {
            int best = -1000000000;
            for (auto& m : moves) {
                Game child = pos; std::string err;
                if (!child.move(m, err)) continue;
                int sc = search(child, depth-1, alpha, beta);
                best = std::max(best, sc);
                alpha = std::max(alpha, sc);
                if (beta <= alpha) break;
            }
            return best;
        } else {
            int best = +1000000000;
            for (auto& m : moves) {
                Game child = pos; std::string err;
                if (!child.move(m, err)) continue;
                int sc = search(child, depth-1, alpha, beta);
                best = std::min(best, sc);
                beta = std::min(beta, sc);
                if (beta <= alpha) break;
            }
            return best;
        }
    }

    std::string select_move(const Game& g0) override {
        Game root = g0; // need non-const for legal_moves()
        auto moves = root.legal_moves();
        if (moves.empty()) return "";

        int bestScore = (g0.side_to_move()==Color::White ? -1000000000 : +1000000000);
        std::string best = moves.front();

        for (auto& m : moves) {
            Game child = g0; std::string err;
            if (!child.move(m, err)) continue;
            int sc = search(child, max_depth-1, -1000000000, +1000000000);
            if (g0.side_to_move()==Color::White) {
                if (sc > bestScore) { bestScore = sc; best = m; }
            } else {
                if (sc < bestScore) { bestScore = sc; best = m; }
            }
        }
        return best;
    }
};

// --------- Game::loop_with_strategies (after Strategy defined) ---------
void Game::loop_with_strategies(Strategy* white, Strategy* black) {
    while (true) {
        std::cout << "\n" << to_cstr(turn) << " to move.\n";
        b.display_board();

        Strategy* who = (turn==Color::White ? white : black);
        std::string line;
        if (who) {
            line = who->select_move(*this);
            if (line.empty()) { std::cout << to_cstr(turn) << " has no move.\n"; break; }
            std::cout << "> " << line << "\n";
        } else {
            std::cout << "Enter move (e.g., 10 30): ";
            if (!std::getline(std::cin, line)) break;
            if (line=="quit" || line=="exit") break;
        }

        std::string err;
        if (!move(line, err)) { std::cout << "Invalid: " << err << "\n"; continue; }

        if (is_checkmate(turn)) { b.display_board(); std::cout << "Checkmate! " << to_cstr(other(turn)) << " wins.\n"; break; }
        if (is_stalemate(turn)) { b.display_board(); std::cout << "Stalemate! Draw.\n"; break; }
        if (in_check(turn))     { std::cout << "Check on " << to_cstr(turn) << "!\n"; }
    }
}

// ==================== main ====================
int main() {
    Game game;
    MinimaxStrategy ai;
    ai.max_depth = 3; // try 2 for speed, 3–4 when optimized
    // You (White) vs AI (Black):
    game.loop_with_strategies(/*white*/nullptr, /*black*/&ai);
    return 0;
}

