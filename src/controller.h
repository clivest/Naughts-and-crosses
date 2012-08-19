#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "wx/wx.h"
#include "game.h"

class sqlite3;

class Controller : public wxApp {
	private:
		sqlite3 *db;
		Game *game;
	public:
		virtual bool OnInit();
		virtual int OnExit();
};

#endif
