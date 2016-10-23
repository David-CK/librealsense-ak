//#include <>
//sleep(10);
//fprintf(stderr, "aaa\n");
//std::cout << "aaa";
//glfwDestroyWindow(win);
#include "example.hpp"
//#include <stdio.h>
//#include <unistd.h>

int main(int argc, char *argv[])
{
    glfwInit();
    GLFWwindow *win = glfwCreateWindow(800, 600, "hello", 0, 0);

    glfwMakeContextCurrent(win);

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(win);
        //draw_text(9, 17, "aa");
        //int w,h;
        //glfwGetFramebufferSize(win, &w, &h);
        //glViewport(0, 0, w, h);
    }
    glfwTerminate();

    return 0;
}

