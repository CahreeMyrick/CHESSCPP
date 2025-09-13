#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>

const int ROWS = 8;
const int COLS = 8;

class Piece{
public:
  std::string color;
  std::string piece_type;
  std::unordered_map<std::string, char> piece_displays {
    {"pawn", 'p'},
    {"knight", 'n'},
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
    this->display = (color=="black") ? piece_displays[piece_type] : (piece_displays[piece_type] - ('a' - 'A'));
  }

};

class Board{
  public:
  Piece board[ROWS][COLS];

  void set_board(){
    std::vector<std::string> colors = {"white", "black"};
    for (std::vector<std::string>::iterator color = colors.begin(); color != colors.end(); color++)
    {
      // set pawns
      for (int j = 0; j < COLS; j++)
      {
        board[(*color=="black" ? 1: 6)][j] = Piece(*color, "pawn");
      }

      // set knights, bishops, rooks
      for (int i = 0; i < 2; i++)
      {
       //knight
        board[(*color=="black") ? 0 : 7][(i==0) ? 1 : 6] = Piece(*color, "knight");

       //bishop
        board[(*color=="black") ? 0 : 7][(i==0 ? 2 : 5)] = Piece(*color, "bishop");
      
        //rook
        board[(*color=="black") ? 0 : 7][(i==0 ? 0 : 7)] = Piece(*color, "rook");
      
      }

      // set king and queen
      board[(*color=="black") ? 0 : 7][3] = Piece(*color, "queen");
      board[(*color=="black") ? 0 : 7][4] = Piece(*color, "king");

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