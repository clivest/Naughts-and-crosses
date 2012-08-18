#include "game.h"
#include "board.h"
#include "piece.h"
#include "naught.h"
#include "cross.h"
#include <sqlite3.h>
#include <sstream>
#include <iostream>
#include "wx/wx.h"

BEGIN_EVENT_TABLE(Game, wxFrame)
    EVT_TIMER(1, Game::OnTimer)
END_EVENT_TABLE()

Game::Game(sqlite3 *db, const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER), size(size), timer(new wxTimer(this, 1)), db(db), moveNum(1) {
	//prep db statements
	//newGame = new sqlite3_stmt();
	//updateGame = new sqlite3_stmt();
	//newMove = new sqlite3_stmt();
	sqlite3_prepare_v2(db, "INSERT INTO game (outcomeid) values(1);", -1, &newGame, NULL);
	sqlite3_prepare_v2(db, "UPDATE game SET outcomeid = @STA WHERE id = @GID", -1, &updateGame, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO move (gameid, playerid, movenum, x, y) VALUES(@GID, @PID, @NUM, @XX, @YY);", -1, &newMove, NULL);
	
	logNewGameToDB();
	
	board = new Board(this, size);
	//no pieces yet
	
	state = NaughtNext;
	CreateStatusBar();
	updateStatus();
}

void Game::OnTimer(wxTimerEvent& event) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (pieces[i][j] != NULL)
				pieces[i][j]->toggleColor();
	board->render();
}

Piece *Game::newPiece(const int i, const int j) {
	Piece *newPiece;
	if (state == NaughtNext)
		newPiece = new Naught();
	else if (state == CrossNext)
		newPiece = new Cross();
	else
		throw "Game has ended, but requesting next piece";
	
	pieces[i][j] = newPiece;
	state = (GameState)!state;
		
	//log move to db
	sqlite3_bind_int(newMove, 1, gameId);
	sqlite3_bind_int(newMove, 2, newPiece->getPlayerId());
	sqlite3_bind_int(newMove, 3, moveNum++);
	sqlite3_bind_int(newMove, 4, i);
	sqlite3_bind_int(newMove, 5, j);
	if (sqlite3_step(newMove) != SQLITE_DONE) {
		std::cout << "Can't insert into move table: " << sqlite3_errmsg(db) << std::endl;
		throw "Can't insert into move table";
	}
	sqlite3_clear_bindings(newMove);
    sqlite3_reset(newMove);
	
	bool gameOver = checkIf3InRow(i, j);
	if (gameOver) {
		if (state == NaughtNext)
			state = CrossWon;
		else
			state = NaughtWon;
		
		//start flashing winning line
		timer->Start(500);
	}
	else {
		gameOver = true;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				gameOver &= pieces[i][j] != NULL;
		if (gameOver)
			state = Draw;
	}
	
	if (gameOver) {
		//update outcome of game in db
		sqlite3_bind_int(updateGame, 1, state);
		sqlite3_bind_int64(updateGame, 2, gameId);
		if (sqlite3_step(updateGame) != SQLITE_DONE) {
			std::cout << "Can't update game table: " << sqlite3_errmsg(db) << std::endl;
			throw "Can't update game table";
		}
		sqlite3_clear_bindings(updateGame);
        sqlite3_reset(updateGame);
	}
	
	updateStatus();
	return newPiece;
}

bool Game::checkIf3InRow(int i, int j) {
	bool isNaught = pieces[i][j]->isNaught();
	bool gameOver = true;
	
	//vertical
	for (int i1 = 0; i1 < 3 && gameOver; i1++)
		gameOver &= (pieces[i1][j] != NULL && !(pieces[i1][j]->isNaught() ^ isNaught));
	if (gameOver) {
		for (int i1 = 0; i1 < 3; i1++)
			pieces[i1][j]->setInWinningLine(true);
		return true;
	}
	
	gameOver = true;
	//horizontal
	for (int j1 = 0; j1 < 3 && gameOver; j1++)
		gameOver &= (pieces[i][j1] != NULL && !(pieces[i][j1]->isNaught() ^ isNaught));
	if (gameOver) {
		for (int j1 = 0; j1 < 3; j1++)
			pieces[i][j1]->setInWinningLine(true);
		return true;
	}
	
	//diagonal from top left
	if (i == j) {
		gameOver = true;
		for (int ij = 0; ij < 3 && gameOver; ij++)
			gameOver &= (pieces[ij][ij] != NULL && !(pieces[ij][ij]->isNaught() ^ isNaught));
		if (gameOver) {
			for (int ij = 0; ij < 3; ij++)
				pieces[ij][ij]->setInWinningLine(true);
			return true;
		}
	}
	
	//diagonal from bottom left
	if (i+j == 2) {
		gameOver = true;
		for (int i1 = 0; i1 < 3 && gameOver; i1++)
			gameOver &= (pieces[i1][2-i1] != NULL && !(pieces[i1][2-i1]->isNaught() ^ isNaught));
		if (gameOver) {
			for (int i1 = 0; i1 < 3; i1++)
				pieces[i1][2-i1]->setInWinningLine(true);
			return true;
		}
	}
	
	return false;
}

void Game::updateStatus() {
	switch (state) {
		case NaughtNext:
			SetStatusText(_("Naughts turn"));
			break;
		case CrossNext:
			SetStatusText(_("Crosses turn"));
			break;
		case NaughtWon:
			SetStatusText(_("Naughts have won! Click to restart"));
			break;
		case CrossWon:
			SetStatusText(_("Crosses have won! Click to restart"));
			break;
		case Draw:
			SetStatusText(_("Game drawn. Click to restart"));
			break;
	}
}

bool Game::inProgress() {
	return state != NaughtWon && state != CrossWon && state != Draw;
}

void Game::restart() {
	//stop timer
	timer->Stop();
	//reset board
	board->reset();
	//delete pieces
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			wxDELETE(pieces[i][j]);
	
	logNewGameToDB();
	//reset state
	state = NaughtNext;
	moveNum = 1;
	updateStatus();
}

void Game::logNewGameToDB() {
	//log new game to database. In progress
	if (sqlite3_step(newGame) != SQLITE_DONE) {
		std::cout << "Can't insert into game table: " << sqlite3_errmsg(db) << std::endl;
		throw "Can't insert into game table";
	}
	gameId = sqlite3_last_insert_rowid(db);
	sqlite3_reset(newGame);
}
