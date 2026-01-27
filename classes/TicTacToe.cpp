#include "TicTacToe.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= -1;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{

    //Check for if the game is still going
    _gameOver = false;

    // Staging the board to prepare for the game
    setNumberOfPlayers(2);
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 3; x++) {
            _grid[y][x].initHolder(ImVec2(100*x + 100, 100*y + 100),"square.png",x,y);
           }
    }
    // check to see if the game has an AI that can play
    if(gameHasAI()){
        setAIPlayer(AI_PLAYER);
    }
    startGame();
}




bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{   
    // Check to see if the is running and if the holder exists and is vaild
    if (_gameOver || !holder || holder->bit())
        return false;
    // Handling player inputs to load it into the bit 
    Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    bit->setPosition(holder->getPosition());
    holder->setBit(bit);

    // Checking to see if the game ends to a draw or a win
    if (checkForWinner() || checkForDraw()) {
        _gameOver = true;
    }

    return true;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{   
    // Stopping the game 
    _gameOver = true;

    // clear out the board
    // loop through the 3x3 array and call destroyBit on each square

    for (int y = 0;y < 3; y++){
        for (int x = 0; x < 3; x++)
        _grid[y][x].destroyBit();
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index ) const
{
    // index is 0..8, convert to x,y using:
    // y = index / 3
    // x = index % 3 
    // returns null if there is no bit found

    if (!_grid[index / 3][index % 3].bit()) {
        return nullptr;
    }

    // and if it does yippee here is the bit
    return _grid[index / 3][index % 3].bit()->getOwner();
}

Player* TicTacToe::checkForWinner()
{

    // and array of all the winning coordinates
    const int WinningTriple[8][3] = {
    {0,1,2},
    {3,4,5},
    {6,7,8},
    {0,3,6},
    {1,4,7},
    {2,5,8},
    {0,4,8},
    {2,4,6}
};
    // you can use the ownerAt helper function to get the owner of a square
    // for example, ownerAt(0) returns the owner of the top-left square
    // if there is no bit in that square, it returns nullptr
    // if you find a winning triple, return the player who owns that triple
    // otherwise return nullptr

    // Using auto to scan throught the array and checking if the board ends up in on of these given states
    for (auto& w : WinningTriple) {
        Player* p = ownerAt(w[0]);
        if (p && p == ownerAt(w[1]) && p == ownerAt(w[2]))
            return p;
    }

    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    // is the board full with no winner?
    // if any square is empty, return false
    // otherwise return true
    for (int y = 0;y < 3; y++){
        for (int x = 0; x < 3; x++)
        if (!_grid[y][x].bit()){
            return false;
        }
    }
    return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const
{

    //Making a sting that can be wriiten into when needed to note down the state of the game
    std::string s;
    for (int y = 0;y < 3; y++){
        for (int x = 0; x < 3; x++) {
            Bit *bit = _grid[y][x].bit();
            if (bit) {
                s += std::to_string(bit->getOwner()->playerNumber()+1);
            } else{
                s += "0";
            }   
        }
    }
    return s;
}

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{

    // Writing into the state string the state of the bit so that the information is accessable in string form when needed
    int i = 0;

        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {

                int player = s[i] - '0';

                if (player == 0)
                    _grid[y][x].setBit(nullptr);
                else
                    _grid[y][x].setBit(PieceForPlayer(player - 1));

                ++i;
            }
        }
}


//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() 
{
    // we will implement the AI in the next assignment!
    std::string currentState = stateString();

    int bestMove =-1000;
    int bestSquare = -4;

    for(int i = 0; i<9; i++) {
        if (currentState[i] == '0'){
            currentState[i] = '2';
            int newValue = -negamax(currentState,2,0,0,HUMAN_PLAYER);
            if(newValue > bestMove) {
                bestSquare = i;
                bestMove = newValue;
            }
        }
    }
}

