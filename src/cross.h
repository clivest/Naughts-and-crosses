#ifndef CROSS_H
#define CROSS_H

#include "piece.h"

class Cross : public Piece {
	public:
		bool isNaught() { return false; };
		void draw(float top_x, float top_y, float width, float height);
};

#endif
