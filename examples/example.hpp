#include <GLFW/glfw3.h>

#include "third_party/stb_easy_font.h"
void draw_text(int x, int y, const char * text)
{
    char buffer[20000]; // ~100 chars
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, 4*stb_easy_font_print((float)x, (float)(y-7), (char *)text, 0, buffer, sizeof(buffer)));
    glDisableClientState(GL_VERTEX_ARRAY);
}

class texture_buffer
{
    GLuint texture;

    void show(float rx, float ry, float rw, float rh)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(rx,    ry   );
        glTexCoord2f(1, 0); glVertex2f(rx+rw, ry   );
        glTexCoord2f(1, 1); glVertex2f(rx+rw, ry+rh);
        glTexCoord2f(0, 1); glVertex2f(rx,    ry+rh);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

