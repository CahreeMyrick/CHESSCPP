#include <iostream>
#include <string>

const int ROWS = 8;
const int COLS = 8;

class Piece{
public:
  std::string color;
  std::string piece_type;
  char display;
  
  // defualt constructor (no args)
  Piece() : color("None"), piece_type("Empty"), display('-') {}

  // 3-arg constuctor
  Piece(std::string color, std::string piece_type, char display) : 
  color(color), piece_type(piece_type), display(display) {}

};

class Board{
  public:
  Piece board[ROWS][COLS];

  void set_pawn(std::string color, int row){
    for (int j = 0; j < COLS; j++){
      Piece p(color, "pawn", 'p');
      board[row][j] = p;
    }
  }

  void set_empty(){
    for (int i = 1; i < 7; i++){
      for (int j = 0; j < 8; j++)
      {
        Piece p("None", "Empty", '-');
        board[i][j] = p;
      }
    }
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
  // Game() : {}

};

int main()
{
  Board b;
  b.display_board();

  return 0;
}