#ifndef SQUARE_H
#define SQUARE_H

class Board;
class Piece;

class Square {
	private:
		Board *board;
		Piece *piece;
		//position of top left corner
		float pos_x;
		float pos_y;
		const float width;
		const float height;
	public:
		Square(Board *board, float pos_x, float pos_y);
		void draw();
		bool isEmpty();
		void setPiece(Piece *piece);
		void reset();
};

#endif
