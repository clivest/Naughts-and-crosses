#include "square.h"
#include "board.h"
#include "piece.h"
#include "wx/glcanvas.h"

Square::Square(Board *board, float pos_x, float pos_y)
	: width(2.0/3.0), height(2.0/3.0), board(board), pos_x(pos_x), pos_y(pos_y) {
	piece = NULL;
}

void Square::draw() {
	//draw box
	glBegin(GL_LINE_LOOP);
		glColor3f(0, 0, 0);
		glLineWidth(1);
		glVertex2f(pos_x, pos_y);
		glVertex2f(pos_x + width, pos_y);
		glVertex2f(pos_x + width, pos_y + height);
		glVertex2f(pos_x, pos_y + height);
	glEnd();
	
	//draw piece
	if (piece != NULL)
		piece->draw(pos_x + width/8, pos_y + height/8, width*3/4, height*3/4);
}

bool Square::isEmpty() {
	return piece == NULL;
}

void Square::setPiece(Piece *piece) {
	this->piece = piece;
}

void Square::reset() {
	piece = NULL;
}
