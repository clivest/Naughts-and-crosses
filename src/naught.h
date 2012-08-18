#ifndef NAUGHT_H
#define NAUGHT_H

#include "piece.h"

class Naught : public Piece {
	public:
		bool isNaught() { return true; };
		void draw(float top_x, float top_y, float width, float height);
};

#endif
