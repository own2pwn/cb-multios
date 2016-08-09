#ifndef SAMPLE_SHIPGAME_H
#define SAMPLE_SHIPGAME_H

//doesn't work for other sizes
#define BOARD_SIZE 10

#if (BOARD_SIZE != 10)
#warn "Only tested for board size of 10"
#endif

typedef struct _Command
{
  char cmdNum;
  //changed from the array into individual 
  char row;
  char col;
  char dir; 
  char shipNum;
}cgc_Command;

typedef struct _PlayerState
{
  int playerNum; //player number
  int ship2Life; //these are life points
  int ship3Life;
  int ship4Life;
  int ship5Life;
  char board[BOARD_SIZE][BOARD_SIZE]; //the board itself
} cgc_PlayerState;

int cgc_getCommand(cgc_Command* cmd);

int cgc_clearPlayerState(cgc_PlayerState* ps);

int cgc_dumpBoard(cgc_PlayerState* ps);

int cgc_processPlace(cgc_PlayerState* ps, cgc_Command* cmd);
int cgc_processTarget(cgc_PlayerState* ps, cgc_Command* cmd);

int cgc_printHelp();
#endif//SAMPLE_SHIPGAME_H
