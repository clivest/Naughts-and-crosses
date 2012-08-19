#include "controller.h"
#include "game.h"
#include "wx/wx.h"
#include <sqlite3.h>
#include <iostream>
#include <string>

bool Controller::OnInit() {
	//database setup
	if (sqlite3_open("db/naughts_and_crosses.db", &db)){
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return false;
	}
	
	if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, 0, NULL) != SQLITE_OK) {
		std::cout << "Can't enable foreign key constraints: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return false;
	}
	
	game = new Game(db, _("Naughts and crosses"), wxDefaultPosition, wxSize(350, 350));
	game->Show(true);
	SetTopWindow(game);
	return true;
}

int Controller::OnExit() {
	int rc = wxApp::OnExit();
	game->endThread();
	sqlite3_close(db);
	return rc;
}
