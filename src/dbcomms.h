#ifndef DBCOMMS_H
#define DBCOMMS_H

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <sqlite3.h>

class DbComms {
	private:
		boost::asio::io_service service;
		//boost::asio::io_service::work *work;
		boost::scoped_ptr<boost::asio::io_service::work> work;
		
		sqlite3 *db;
		sqlite3_stmt *newGameStmt;
		sqlite3_stmt *updateGameStmt;
		sqlite3_stmt *newMoveStmt;
		
		mutable sqlite_int64 gameId;
	public:
		DbComms(sqlite3 *db) : db(db), service(), work(new boost::asio::io_service::work(service)) { }
		~DbComms() { sqlite3_finalize(newGameStmt); sqlite3_finalize(updateGameStmt); sqlite3_finalize(newMoveStmt); };
		
		void newGameOp() const;
		void updateGameOp(int state) const;
		void newMoveOp(int playerId, int moveNum, int i, int j) const;

		void newGame();
		void updateGame(int state);
		void newMove(int playerId, int moveNum, int i, int j);

		void loop();
		void workFinished();
};

#endif
