#include <iostream>
#include <string>
#include <unordered_map>


const int ROWS = 8;
const int COLS = 8;

class Piece{
public:
  std::string color;
  std::string piece_type;
  std::unordered_map<std::string, char> piece_displays {
    {"pawn", 'p'},
    {"kngiht", 'n'},
    {"bishop", 'b'},
    {"king", 'k'},
    {"queen", 'q'},
    {"rook", 'r'},
    {"empty", '-'}
  };
  char display;

  
  // defualt constructor (no args)
  Piece() : color("None"), piece_type("Empty"), display('-') {}

  // 2-arg constuctor
  Piece(std::string color, std::string piece_type){
    this->color=color;
    this->piece_type=piece_type;
    this->display = piece_displays[piece_type];
  }

};

class Board{
  public:
  Piece board[ROWS][COLS];

  void set_pawn(std::string color, int row){
    for (int j = 0; j < COLS; j++){
      Piece p(color, "pawn");
      board[row][j] = p;
    }
  }

  void set_empty(){
    for (int i = 1; i < 7; i++){
      for (int j = 0; j < 8; j++)
      {
        Piece p("None", "Empty");
        board[i][j] = p;
      }
    }
  }

  void set_board(){

    // brute force alg, present all possible combinations and traverse each one
     std::unordered_map<std::string, int> pawn_presets{
        {"white", 1},
        {"black", 6}
     };
    
    // set white pawn
      for (auto it = pawn_presets.begin(); it != pawn_presets.end(); it++)
      {
        for (int j = 0; j < COLS; j++)
        {
          Piece p(it->first, "pawn");
          board[it->second][j] = p;
        }
      }

      /*
      // set minor pieces
      Piece p("white", "knight");
      board[0][1] = p;

      Piece p("white", "kngiht");
      board[0][6] = p;

      Piece p("black", "knight");
      board[7][1] = p;
      */
  }

  void display_board(){
    for (int i = 0; i < ROWS; i++){
      for (int j = 0; j < COLS; j++){
        Piece piece = board[i][j];
        std::cout << piece.display << " ";
      }
      std::cout << std::endl;
    }
  }

};

class DayofYear{
  int output();
  int month;
};

class Game{
public:

  void user_interaction(){
    std::cout << "----- WELCOME TO CHESSMANIA -----" << std::endl;
    std::cout << "What game mode would you like to play?" << std::endl;
    std::cout << "1. Player vs. Player (PVP)" << std::endl;
    std::cout << "2. Player vs AI Bot (PVB)" << std::endl;
    std::cout << "3. Player vs Creator (PVC)" << std::endl;
  }

  void start_game(){
    // create and set board
    Board b;
    b.set_board();
    b.display_board();

  }

  

};

int main()
{
  Game game;
  game.start_game();

  return 0;
}