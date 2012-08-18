#include "naught.h"
#include "wx/glcanvas.h"
#include <algorithm>
#include <cmath>

#define PI 3.14159265

void Naught::draw(float top_x, float top_y, float width, float height) {
	float rad = std::max(width, height) / 2,
		cen_x = top_x + rad,
		cen_y = top_y + rad;
		
	glBegin(GL_LINE_LOOP);
		glColor3f(r, g, b);
		for (float i = 0.0; i < 2 * PI; i += (2 * PI)/100)
			glVertex2f(cen_x - rad*sin(i), cen_y - rad*cos(i));
	glEnd();
}
