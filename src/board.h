#ifndef BOARD_H
#define BOARD_H

#include "wx/glcanvas.h"
#include "wx/wx.h"

class Square;
class Game;

class Board : public wxGLCanvas {
	private:
		Square *squares[3][3];
		Game *game;
	public:
		Board(Game *parent, const wxSize& size);
		~Board();
		void OnPaint(wxPaintEvent& WXUNUSED(event));
		void OnClick(wxMouseEvent& event);
		void OnTimer(wxTimerEvent& event);
		void render();
		void reset();
	protected:
	    DECLARE_EVENT_TABLE()
};

#endif
