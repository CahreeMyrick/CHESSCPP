Requirements
The goal of this project is to build a software that can run a chess match between either
between two players(PVP), or between a player and an AI bot (PVB).

Player vs Player (PVP)
• This is one of two major components of the software. We cannot implement the player vs bot (PVB)
mode until after this is completley and accurately implemented .

PVP Design:
  • The current approach I have in mind to desgin and implement the PVP componenet is to 
  leverage an object oriented approach to handle pieces, the board, and the game state. 

  class Board: 
  • The purpose of this class will be to store and maintain the board state.
    Member Variables:
      - board: 8x8 2d array:
        Each piece on the board will be represented by its own class which inherits from the parent class _Piece_

  class Piece:
  • This will be a parent class to each piece. Each different chess piece will inherit from this base class
  and contain their specific movement logic.

  class Pawn
  class Rook
  class Queen
  class King  
  class Bishop
  class Knight

  class Game:
  • This class will handle the main game loop. This includes recieving input, making moves, alternating turns etc.
  

  9/12/25
  Checkpoint reached: set and display board

  current design:

  class Piece
  member vars:
  - std::string color
    • The color of the piece
  - std::string piece_type
    • The type of the piece (i.e., pawn, bishop, rook)
  - std::unordered_map<std::string, char> piece_displays
    • A dictionary contianing the piece names and their display characters (could maybe go outside of this class)
  - char display
    • Holds the display char (i.e, 'p', 'r', 'K')

class Board
  member vars:
  Piece board[ROWS][COLS];
  memeber methods:
  - void set_board()
    • sets the board as a 2d array such that each piece is an object of the Piece class
  - void display_board()
    • display the board to terminal
  
class Game
  member methods:
  - void start_game()

**Next Steps:** Implement Movement logic
development approach:
  - start with pawn -> test
  - then knight->test
  -  then other pieces ....
  -  casteling, enpassent, checks, etc come after basic fucntionality implementtation

9/14/25
- implemented the pawn movement logic yesterday, moving onto knight movement logic today

9/15/25
- I think the design in the minimax.cpp is better since it enables polymorphism and allows each piece to contain its own movement logic.
- I also think its a more efficent design for when I dive into more AI implementaions
- I also think using pointers/dynamic memeory may be a better approach for when I dont know objects at compile time.

The minmiax.cpp file curently has
- full game logic
- AI implementation (minimax with alpha beta bruning)

How do I want this to scale?
- not currently sure
- I want it to grow into a larger software product
- need to learn more about software engineering principles/scaling software, deploying projects




9/17/25

Class Piece{
public:

// member vars (common to all pieces)
Color color = Color::None // piece color
bool hasMoved = false; // for castling and double pawn steps

// constructor
Piece(Color col=Color::None) : color(col) {} // inializes the color to defualt "None"

// virtual destructor
virtual ~Piece() = default; 
• virtual destructor: necessary whenevery you destory/delete derived objects via base pointer/reference, without it youd get undefined behavior

// polymorphic display
virtual std::string display() {return "?"}
• lets each derived class (king, queen, pawn etc ) override how it shows itself.

virtual bool can_move(const Board&b, int r0, int c0, int r1, int c1) = 0;
• pure virtual, so Piece is abstract and derived classses must implement their move logic. Niccessary if you want compile time enforcement that every real piece defines its movement

  
    
      
  
