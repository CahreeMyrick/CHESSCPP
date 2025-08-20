#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <cmath>

const int ROWS = 8;
const int COLS = 8;

class Board; // forward declare so we can reference it in Piece

// ==================== Piece base ====================
class Piece {
public:
    std::string color; // "white", "black", or "none" for empty
    std::vector<std::string> all_available_moves; // (unused here)
    Piece(std::string col = "none") : color(std::move(col)) {}
    virtual ~Piece() = default;
    virtual std::string display() { return "?"; }

    // Pseudo-legal: does not check for self-check, etc.
    virtual bool can_move(const Board &b, int r0, int c0, int r1, int c1) const = 0;
};

// ==================== Derived pieces ====================
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

    void display_board() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                std::cout << board[i][j]->display() << " ";
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

    ~Board() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                delete board[i][j]; // delete nullptr is OK
            }
        }
    }
};

// ==================== Movement logic ====================
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

    return false; // no en passant / promotions here
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
    return std::max(dr, dc) == 1; // no castling here
}

// ==================== Move logic ====================
class Move {
    static int c2i(char c) {
        if (c < '0' || c > '7') throw std::out_of_range("index not 0-7");
        return c - '0';
    }

public:
    std::vector<std::string> get_move() {
        std::string user_move;
        std::cout << "Enter Move (e.g., P10 30 or 10 30): ";
        std::getline(std::cin, user_move);

        std::stringstream ss(user_move);
        std::string token;
        char delimeter = ' ';
        std::vector<std::string> result;

        while (std::getline(ss, token, delimeter)) {
            if (!token.empty()) result.push_back(token);
        }

        if (result.size() != 2) {
            throw std::runtime_error("Expected format like: P10 30 or 10 30");
        }
        if (!(result[0].size() == 2 || result[0].size() == 3)) {
            throw std::runtime_error("Origin must be like Pxy or xy (e.g., P10 or 10)");
        }
        if (result[1].size() != 2) {
            throw std::runtime_error("Destination must be like xy (e.g., 30)");
        }

        return result;
    }

    // (kept for potential future use)
    Piece* get_piece_type(char piece, const std::string& color)  {
        switch (std::tolower(static_cast<unsigned char>(piece))) {
            case 'p': return new Pawn(color);
            case 'n': return new Knight(color);
            case 'b': return new Bishop(color);
            case 'r': return new Rook(color);
            case 'q': return new Queen(color);
            case 'k': return new King(color);
            default:  return nullptr;
        }
    }

    void make_move(const std::vector<std::string>& user_move, Board& b) {
        const std::string& orig = user_move[0]; // e.g., "P10" or "10"
        const std::string& dest = user_move[1]; // e.g., "30"

        // allow "P10" or "10"
        int ooff = std::isdigit(static_cast<unsigned char>(orig[0])) ? 0 : 1;
        if (orig.size() < ooff + 2 || dest.size() < 2) throw std::runtime_error("Bad format");
        int r0 = c2i(orig[ooff]), c0 = c2i(orig[ooff + 1]);
        int r1 = c2i(dest[0]),    c1 = c2i(dest[1]);

        if (!b.in_bounds(r0, c0) || !b.in_bounds(r1, c1)) throw std::out_of_range("Out of bounds");

        Piece*& src = b.board[r0][c0];
        if (src == nullptr || dynamic_cast<Empty_Square*>(src) != nullptr)
            throw std::runtime_error("No piece at origin");

        if (!src->can_move(b, r0, c0, r1, c1))
            throw std::runtime_error("Illegal move for that piece");

        // Can't capture own piece
        if (b.is_friend(r1, c1, src->color))
            throw std::runtime_error("Cannot capture own piece");

        // Move: delete captured target, move pointer, make origin empty
        delete b.board[r1][c1];
        b.board[r1][c1] = src;
        src = new Empty_Square();
    }
};

// ==================== main ====================
int main() {
    Board my_board;
    my_board.create_board();
    my_board.display_board();

    Move mv;

    try {
        auto user_move = mv.get_move();   // e.g., P10 30  or  10 30
        mv.make_move(user_move, my_board);
        std::cout << "\nAfter move:\n";
        my_board.display_board();
    } catch (const std::exception& e) {
        std::cerr << "Move error: " << e.what() << "\n";
    }

    return 0;
}
