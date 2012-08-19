#ifndef GAME_H
#define GAME_H

#include "wx/wx.h"
#include <sqlite3.h>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

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
class DbComms;

class Game : public wxFrame {
	private:
		Board *board;
		Piece * pieces[3][3];
		wxTimer *timer;
		const wxSize& size;
		
		DbComms *dbComms;
		boost::thread t;

		int moveNum;
		GameState state;
		
		bool checkIf3InRow(int i, int j);
		void logNewGameToDB();
	public:
		Game(sqlite3 *db, const wxString& title, const wxPoint& pos, const wxSize& size);
		~Game();
		void endThread();
		Piece *newPiece(int i, int j);
		void updateStatus();
		bool inProgress();
		void restart();
		void OnTimer(wxTimerEvent& event);
	protected:
	    DECLARE_EVENT_TABLE()
};

#endif
