#include "dbcomms.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <sqlite3.h>

void DbComms::newGameOp() const { 
	std::cout << "Logging new game" << std::endl;
	if (sqlite3_step(newGameStmt) != SQLITE_DONE) {
		std::cout << "Can't insert into game table: " << sqlite3_errmsg(db) << std::endl;
		throw "Can't insert into game table";
	}
	gameId = sqlite3_last_insert_rowid(db);
	sqlite3_reset(newGameStmt);
}

void DbComms::updateGameOp(int state) const { 
	std::cout << "Logging update game" << std::endl;
	//update outcome of game in db
	sqlite3_bind_int(updateGameStmt, 1, state);
	sqlite3_bind_int64(updateGameStmt, 2, gameId);
	if (sqlite3_step(updateGameStmt) != SQLITE_DONE) {
		std::cout << "Can't update game table: " << sqlite3_errmsg(db) << std::endl;
		throw "Can't update game table";
	}
	sqlite3_clear_bindings(updateGameStmt);
    sqlite3_reset(updateGameStmt);
}

void DbComms::newMoveOp(int playerId, int moveNum, int i, int j) const { 
	std::cout << "Logging new move" << std::endl;
	sqlite3_bind_int(newMoveStmt, 1, gameId);
	sqlite3_bind_int(newMoveStmt, 2, playerId);
	sqlite3_bind_int(newMoveStmt, 3, moveNum);
	sqlite3_bind_int(newMoveStmt, 4, i);
	sqlite3_bind_int(newMoveStmt, 5, j);
	if (sqlite3_step(newMoveStmt) != SQLITE_DONE) {
		std::cout << "Can't insert into move table: " << sqlite3_errmsg(db) << std::endl;
		throw "Can't insert into move table";
	}
	sqlite3_clear_bindings(newMoveStmt);
	sqlite3_reset(newMoveStmt);
}

void DbComms::newGame() {
	service.post(boost::bind(&DbComms::newGameOp, this));
}

void DbComms::updateGame(int state) {
	service.post(boost::bind(&DbComms::updateGameOp, this, state));
}

void DbComms::newMove(int playerId, int moveNum, int i, int j) {
	service.post(boost::bind(&DbComms::newMoveOp, this, playerId, moveNum, i, j));
}

void DbComms::loop()	{
	//prep db statements
	sqlite3_prepare_v2(db, "INSERT INTO game (outcomeid) values(1);", -1, &newGameStmt, NULL);
	sqlite3_prepare_v2(db, "UPDATE game SET outcomeid = @STA WHERE id = @GID", -1, &updateGameStmt, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO move (gameid, playerid, movenum, x, y) VALUES(@GID, @PID, @NUM, @XX, @YY);", -1, &newMoveStmt, NULL);
	
	service.run(); // processes the tasks
}

void DbComms::workFinished() {
	work.reset();
}
