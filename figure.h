#pragma once
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>


class figure
{
   

public:
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };
	virtual int Paint() = 0 {};
};

