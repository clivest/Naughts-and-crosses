#ifndef PIECE_H
#define PIECE_H

class Piece {
	protected:
		bool inWinningLine;
		float r;
		float g;
		float b;
	public:
		Piece() : inWinningLine(false), r(0.0), g(0.0), b(0.0) {};
		virtual void draw(float top_x, float top_y, float width, float height)=0;
		virtual bool isNaught()=0;
		void setInWinningLine(bool inLine) { inWinningLine = inLine; };
		void toggleColor();
		virtual int getPlayerId() { return 2 - isNaught(); };
};

#endif
