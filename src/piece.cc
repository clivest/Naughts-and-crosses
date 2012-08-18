#include "piece.h"

void Piece::toggleColor() {
	if (inWinningLine)
		r = int(r) ^ 1;
}
