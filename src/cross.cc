#include "cross.h"
#include "piece.h"
#include "wx/wx.h"
#include "wx/glcanvas.h"

void Cross::draw(float top_x, float top_y, float width, float height) {
	glBegin(GL_LINES);
		glColor3f(r, g, b);
		glVertex2f(top_x, top_y);
		glVertex2f(top_x + width, top_y + height);
		glVertex2f(top_x + width, top_y);
		glVertex2f(top_x, top_y + height);
	glEnd();
}
