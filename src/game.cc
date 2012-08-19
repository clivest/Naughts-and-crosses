#include "game.h"
#include "board.h"
#include "piece.h"
#include "naught.h"
#include "cross.h"
#include "dbcomms.h"
#include "wx/wx.h"

BEGIN_EVENT_TABLE(Game, wxFrame)
    EVT_TIMER(1, Game::OnTimer)
END_EVENT_TABLE()

Game::Game(sqlite3 *db, const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER), size(size), timer(new wxTimer(this, 1)), moveNum(1), dbComms(new DbComms(db)) {
	// launches a thread that handles communication with a database
	t = boost::thread(boost::bind(&DbComms::loop, dbComms));
	
	//log the new game to the db on the worker thread
	dbComms->newGame();

	board = new Board(this, size);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			pieces[i][j] = NULL;
	
	state = NaughtNext;
	CreateStatusBar();
	updateStatus();
}

Game::~Game() {
	//delete pieces
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			wxDELETE(pieces[i][j]);
	//delete timer
	delete timer;
}

void Game::endThread() {
	//end thread
	dbComms->workFinished();
	t.join();
	//delete database comms
	delete dbComms;
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
	dbComms->newMove(newPiece->getPlayerId(), moveNum++, i, j);
	
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
	
	if (gameOver)
        dbComms->updateGame(state);
	
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
	
	dbComms->newGame();
	//reset state
	state = NaughtNext;
	moveNum = 1;
	updateStatus();
}
