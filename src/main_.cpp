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



class Move{
public:
  bool in_bounds(int r, int c) { return (0 <= r && r < 8 && 0 <= c && c < 8); }

  std::vector<std::vector<int>> gen_pawn(const Board&B, int r, int c, std::string color)
  {
    std::vector<std::vector<int>> mv;
    int dir = (color=="white") ? -1 : 1;
    int start_rank = (color=="white") ? 6 : 1;
    int one_r = r + dir;

    // single push
    if (in_bounds(one_r, c) && B.board[one_r][c].display=='-') {
        mv.push_back({one_r,c});

        // double push
        int two_r = r + 2*dir;
        if (r == start_rank && in_bounds(two_r,c) && B.board[two_r][c].display == '-') {
            mv.push_back({two_r,c});
        }
    }
    // captures
    for (int dc : {-1, +1}) {
        int rr = r + dir, cc = c + dc;
        if (in_bounds(rr,cc) && B.board[rr][cc].color != color) {
            mv.push_back({rr,cc});
        }
    }
    // (En passant & promotion omitted to keep it minimal)
    return mv;
  }

  std::vector<std::vector<int>> generate_moves_at(const Board& B, int r, int c){
    const Piece& P = B.board[r][c];
    if (P.piece_type == "empty" || P.display=='-') return {};
    std::string color = P.color;

    if (P.piece_type=="pawn")
    {
      return gen_pawn(B, r, c, color);
    }

  }

};

class Game{
public:
  std::string turn = "white";

  void start_game(){
    std::cout << "----- WELCOME TO CHESSMANIA -----" << std::endl;
    std::cout << "What game mode would you like to play?" << std::endl;
    std::cout << "1. Player vs. Player (PVP)" << std::endl;
    std::cout << "2. Player vs AI Bot (PVB)" << std::endl;
    std::cout << "3. Player vs Creator (PVC)" << std::endl;

    std::string game_mode;
    std::cin >> game_mode;

    if (game_mode=="PVP")
    {
      // create and set board
      Board b;
      b.set_board();
      b.display_board();

      Move m;

      std::cout << "It's " << turn << "'s turn. Make your move! " << std::endl;

      std::string move;
      std::vector<std::vector<int>> valid_moves;

      std::cin >> move;
      int r = move[0]-'0';
      int c = move[1]-'0';
      valid_moves = m.generate_moves_at(b, r, c);
      
      for (size_t i = 0; i < valid_moves.size(); i++)
      {
          if ((move[2]-'0' == valid_moves[i][0]) && (move[3]-'0' == valid_moves[i][1]))
          {
            std::cout << "HERE" << std::endl;
            b.board[move[2]-'0'][move[3]-'0'] = std::move(b.board[r][c]);
            b.board[r][c] = Piece();
            b.display_board();
          }

      }

      


    }

  }

  

};

int main()
{
  Game game;
  game.start_game();

  return 0;
}