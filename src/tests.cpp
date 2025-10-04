// test_chess.cpp
#include <cassert>
#include <string>
#include <iostream>

#define CHESS_NO_MAIN
#include "minimax.cpp"

// ---------- helpers ----------
static bool do_ok(Game& g, const std::string& mv) {
    std::string err;
    bool ok = g.move(mv, err);
    if (!ok) std::cerr << "Expected OK, but got: '" << err << "' for move " << mv << "\n";
    return ok;
}
static bool do_bad(Game& g, const std::string& mv) {
    std::string err;
    bool ok = g.move(mv, err);
    if (ok) std::cerr << "Expected FAIL, but move succeeded: " << mv << "\n";
    return !ok;
}
static char at(const Game& g, int r, int c) {
    const Board& b = g.get_board();
    const Piece* p = b.board[r][c].get();
    if (!p) return '-';
    return p->display().empty() ? '?' : p->display()[0];
}
static Color turn(const Game& g) { return g.side_to_move(); }
static std::string col(Color c){ return c==Color::White?"White":c==Color::Black?"Black":"None"; }

// ---------- tests ----------
void test_initial_setup() {
    Game g;
    // Pawns
    for (int c=0;c<8;++c) {
        assert(at(g,1,c) == 'P');
        assert(at(g,6,c) == 'p');
    }
    // Majors (only check a few key squares)
    assert(at(g,0,4) == 'K'); // white king e1
    assert(at(g,0,3) == 'Q'); // white queen d1
    assert(at(g,7,4) == 'k'); // black king e8
    assert(at(g,7,3) == 'q'); // black queen d8

    // Side to move
    assert(turn(g) == Color::White);
}

void test_simple_move_and_turn() {
    Game g;
    // e2->e4 : r1c4 -> r3c4 => "14 34"
    assert(do_ok(g, "14 34"));
    assert(at(g,3,4)=='P' && at(g,1,4)=='-');
    assert(turn(g) == Color::Black);

    // Black e7->e5 : r6c4 -> r4c4 => "64 44"
    assert(do_ok(g, "64 44"));
    assert(at(g,4,4)=='p' && at(g,6,4)=='-');
    assert(turn(g) == Color::White);
}

void test_illegal_move_rejected() {
    Game g;
    // Try moving a knight like a bishop (illegal)
    // g1->g3 is "06 26" (two up same file) — illegal for a knight
    assert(do_bad(g, "06 26"));
    // Board unchanged at source
    assert(at(g,0,6)=='N');
    // Turn should not flip on invalid move
    assert(turn(g) == Color::White);
}

void test_en_passant() {
    Game g;
    // Set up: 1. e2e4  ... d7d5
    assert(do_ok(g, "14 34")); // e2->e4
    assert(do_ok(g, "63 43")); // d7->d5

    // 2. e4e5  ... f7f5  (sets EP target at (5,5))
    assert(do_ok(g, "34 44")); // e4->e5 (white pawn now at r4,c4)
    assert(do_ok(g, "65 45")); // f7->f5 (black pawn now at r4,c5), EP available

    // 3. e5xf6 e.p.: white pawn from (4,4) -> (5,5), capturing pawn at (4,5)
    assert(do_ok(g, "44 55"));

    // Verify: destination has white pawn, the black pawn behind is gone.
    assert(at(g,5,5) == 'P');   // white pawn landed
    assert(at(g,4,5) == '-');   // captured pawn removed
    assert(turn(g) == Color::Black);
}

void test_kingside_castling_white() {
    Game g;

    // Clear path: move knight g1 (0,6) -> e2 (2,5), bishop f1 (0,5) -> c4 (3,2)
    // Knight: "06 25" (0,6)->(2,5)
    assert(do_ok(g, "06 25"));
    // Black play a quiet move to pass turn (a7a6: "60 50")
    assert(do_ok(g, "60 50"));

    // Bishop: "05 32" (0,5)->(3,2)
    assert(do_ok(g, "05 32"));
    // Black pass: "61 51" (b7b6)
    assert(do_ok(g, "61 51"));

    // Now squares f1 (0,5) and g1 (0,6) should be empty
    assert(at(g,0,5)=='-');
    assert(at(g,0,6)=='-');

    // Try castling: King e1->g1 is (0,4)->(0,6): "04 06"
    assert(do_ok(g, "04 06"));

    // Verify king and rook landed on g1 and f1
    assert(at(g,0,6) == 'K');
    assert(at(g,0,5) == 'R');
    assert(at(g,0,4) == '-');
    assert(at(g,0,7) == '-');
    assert(turn(g) == Color::Black);
}

void test_deep_copy_independence() {
    Game g;
    Game h = g; // deep copy

    // Make a move in g (e2e4)
    assert(do_ok(g, "14 34"));
    // h should remain at start position
    assert(at(h,1,4) == 'P' && at(h,3,4) == '-');
    // g changed
    assert(at(g,1,4) == '-' && at(g,3,4) == 'P');
}

void test_legal_moves_nonempty_start() {
    Game g;
    auto lm = g.legal_moves();
    // At least one pawn double-step etc.
    assert(!lm.empty());
    // Example: "10 30" (a2a4) should be in there or some legal like e2e4.
    // We won't assert exact move text; engines can vary. Just ensure non-empty.
}

int main() {
    std::cout << "Running tests...\n";

    test_initial_setup();
    test_simple_move_and_turn();
    test_illegal_move_rejected();
    test_en_passant();
    test_kingside_castling_white();
    test_deep_copy_independence();
    test_legal_moves_nonempty_start();

    std::cout << "All tests passed!\n";
    return 0;
}

