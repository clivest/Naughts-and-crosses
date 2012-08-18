#ifndef GAME_H
#define GAME_H

#include "wx/wx.h"
#include <sqlite3.h>

enum GameState {
	//same as in db table outcomes
	NaughtNext = 0,
	CrossNext = 1,
	NaughtWon = 2,
	CrossWon = 3,
	Draw = 4
};

class Board;
class Piece;

class Game : public wxFrame {
	private:
		Board *board;
		Piece * pieces[3][3];
		wxTimer *timer;
		const wxSize& size;
		
		sqlite3 *db;
		sqlite3_stmt *newGame;
		sqlite3_stmt *updateGame;
		sqlite3_stmt *newMove;
		
		sqlite3_int64 gameId;
		int moveNum;
		GameState state;
		
		bool checkIf3InRow(int i, int j);
		void logNewGameToDB();
	public:
		Game(sqlite3 *db, const wxString& title, const wxPoint& pos, const wxSize& size);
		Piece *newPiece(int i, int j);
		void updateStatus();
		bool inProgress();
		void restart();
		void OnTimer(wxTimerEvent& event);
	protected:
	    DECLARE_EVENT_TABLE()
};

#endif
