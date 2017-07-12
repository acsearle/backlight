#include "Global.hpp"

#ifdef DISPLAY

// This code originally used for low-lwvel acess to SVGA on IBM PCs. Now
// extended to OpenGL
// This is VERY primitive code - the window will not handle propery! Don't try
// to use on
// X-Windows systems yet - probably won't work!

#ifdef PC
#include "windows.h"
#endif

#include "gl\gl.h"
#include "gl\glaux.h"

void SetMode(long w, long h) {
  auxInitDisplayMode(AUX_SINGLE | AUX_RGBA);
  auxInitPosition(0, 0, w, h);
  auxInitWindow("Backlight Preview");

  glOrtho(0.0f, (float)w, 0.0f, (float)h, 1.0f, -1.0f);

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
  glVertex2i(0, 0);
  glVertex2i(w, h);
  glVertex2i(w, 0);
  glVertex2i(0, h);
  glEnd();

  glFlush();

  glBegin(GL_POINTS);
}

void ResetMode(long w, long h) {
  glEnd();

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
  glVertex2i(0, 0);
  glVertex2i(w, h);
  glVertex2i(w, 0);
  glVertex2i(0, h);
  glEnd();

  glFlush();

  glBegin(GL_POINTS);
}

void SetPixel(long u, long v, cIColour c) {
  glColor3ub(c.r(), c.g(), c.b());
  // glBegin(GL_POINTS);
  glVertex2i(u, v);
  // glEnd();
  // glFlush();
}

#endif
