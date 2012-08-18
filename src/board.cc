#include "board.h"
#include "square.h"
#include "game.h"
#include "wx/wx.h"
#include "wx/glcanvas.h"
#include <algorithm>

BEGIN_EVENT_TABLE(Board, wxGLCanvas)
    EVT_PAINT(Board::OnPaint)
    EVT_LEFT_UP(Board::OnClick)
END_EVENT_TABLE()

Board::Board(Game *parent, const wxSize& size) : wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, size, 0, wxT("GLCanvas")), game(parent) {
	//create squares
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			squares[i][2 - j] = new Square(this, float(i)*2/3 - 1, float(j)*2/3 - 1);
}

Board::~Board() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
 			wxDELETE(squares[i][j]);
}

void Board::reset() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
 			squares[i][j]->reset();
 	render();
}

void Board::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	render();
}

void Board::OnClick(wxMouseEvent& event) {
	long x = event.GetX(),
		 y = event.GetY();
	int i = std::min(x*3/GetSize().x, 2L),
		j = std::min(y*3/GetSize().y, 2L);
		
	Square *clicked = squares[i][j];
	if (game->inProgress() && clicked->isEmpty()) {
		Piece *newPiece = game->newPiece(i, j);
		clicked->setPiece(newPiece);
		render();
	}
	else if (!game->inProgress())
		game->restart();
}

void Board::render() {
	SetCurrent();
	wxPaintDC(this);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, (GLint)GetSize().x, (GLint)GetSize().y);
	
	//background
	glBegin(GL_POLYGON);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
	glEnd();
	
	//squares
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
 			squares[i][j]->draw();
 		
	glFlush();
	SwapBuffers();
}
