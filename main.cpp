// Рисует треугольник, цвет которого можно менять с клавиатуры
using namespace std;

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include "key.h"
#include "triangle.h"
#include "gradient.h"
#include "cube.h"

int main()
{
	auto t = cube();
	t.Paint();
}