#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <utility>

const int ROWS = 8;
const int COLS = 8;

class Board; // forward declare so we can reference it in Piece

class Piece {
public:
    std::string color;                 // "white", "black", or "none" for empty
    std::vector<std::string> all_available_moves;
    Piece(std::string col = "none") : color(std::move(col)) {}
    virtual ~Piece() {}
    virtual std::string display() { return "?"; }
};

class Pawn : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "P" : "p"; }
};

class Knight : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "N" : "n"; }
};

class Bishop : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "B" : "b"; }
};

class Rook : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "R" : "r"; }
};

class Queen : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "Q" : "q"; }
};

class King : public Piece {
public:
    using Piece::Piece;
    std::string display() override { return (color == "white") ? "K" : "k"; }
};

class Empty_Square : public Piece {
public:
    Empty_Square() : Piece("none") {}
    std::string display() override { return "-"; }
};

class Board {
public:
    Piece* board[ROWS][COLS]{};

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
        // Fill all with empties first (defensive)
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                board[i][j] = new Empty_Square();

        set_major_pieces("white", 0);
        set_major_pieces("black", 7);
    }

    void display_board() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                std::cout << board[i][j]->display() << " ";
            }
            std::cout << std::endl;
        }
    }

    ~Board() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                delete board[i][j];
            }
        }
    }
};

class Move {
    static int c2i(char c) {
        if (c < '0' || c > '7') throw std::out_of_range("index not 0-7");
        return c - '0';
    }

public:
    std::vector<std::string> get_move() {
        std::string user_move;
        std::cout << "Enter Move (e.g., P10 30): ";
        std::getline(std::cin, user_move);

        std::stringstream ss(user_move);
        std::string token;
        char delimeter = ' ';
        std::vector<std::string> result;

        while (std::getline(ss, token, delimeter)) {
            if (!token.empty()) result.push_back(token);
        }

        if (result.size() != 2) {
            throw std::runtime_error("Expected format like: P10 30");
        }

        if (result[0].size() != 3) {
            throw std::runtime_error("Origin must be like Pxy (e.g., P10)");
        }
        if (result[1].size() != 2) {
            throw std::runtime_error("Destination must be like xy (e.g., 30)");
        }

        return result;
    }

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

    void move(){


    }
    
    void make_move(const std::vector<std::string>& user_move, Board& board_obj) {
        const std::string& orig = user_move[0]; // e.g., "P10"
        const std::string& dest = user_move[1]; // e.g., "30"

        char piece_char = orig[0];
        int orow = c2i(orig[1]);
        int ocol = c2i(orig[2]);
        int nrow = c2i(dest[0]);
        int ncol = c2i(dest[1]);

        // bounds check (redundant with c2i but clearer intent)
        auto in_bounds = [](int r, int c) {
            return r >= 0 && r < ROWS && c >= 0 && c < COLS;
        };
        if (!in_bounds(orow, ocol) || !in_bounds(nrow, ncol))
            throw std::out_of_range("square out of bounds");

        Piece*& src = board_obj.board[orow][ocol];
        Piece*& dst = board_obj.board[nrow][ncol];

        // Get color from source piece (if it's empty, that's an error)
        if (dynamic_cast<Empty_Square*>(src) != nullptr) {
            throw std::runtime_error("No piece at origin square");
        }
        std::string color = src->color;

        // Replace destination with the moved piece (correct derived type)
        delete dst;
        dst = get_piece_type(piece_char, color);
        if (!dst) throw std::runtime_error("Unknown piece type");

        // Set origin to empty
        delete src;
        src = new Empty_Square();
    }
};

int main() {
    Board my_board;
    my_board.create_board();
    my_board.display_board();

    Move mv;

    try {
        auto user_move = mv.get_move();   // e.g., P10 30  (move white pawn from (1,0) to (3,0))
        mv.make_move(user_move, my_board);
        std::cout << "\nAfter move:\n";
        my_board.display_board();
    } catch (const std::exception& e) {
        std::cerr << "Move error: " << e.what() << "\n";
    }

    return 0;
}
