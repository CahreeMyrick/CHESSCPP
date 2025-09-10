Requirements
The goal of this project is to build a software that can run a chess match between either
between two players(PVP), or between a player and an AI bot (PVB).

Player vs Player (PVP)
This is one of two major components of the software. We cannot implement the player vs bot (PVB)
mode until after this is completley and accurately implemented .

PVP Design:
  The current approach I have in mind to desgin and implement the PVP componenet is to 
  leverage an object oriented approach to handle pieces, the board, and the game state. 

  class Board: 
  The purpose of this class will be to store and maintain the board state.

  Member Variables:
    - board: 8x8 2d array:
      Each piece on the board will be represented by its own class which inherits from the parents class
  
