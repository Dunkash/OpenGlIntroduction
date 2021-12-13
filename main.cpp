// Рисует треугольник, цвет которого можно менять с клавиатуры
using namespace std;
#include <iostream>

#include "key.h"
#include "triangle.h"
#include "gradient.h"
#include "cube.h"
#include "pentagon.h"
#include "yellow_cube.h"
#include "textures_file.h"
#include "ellipse.h"
#include "textures_double.h"
#include "lighting_file.h"
#include "figure_model.h"

int main()
{
	//auto f = figure_model("model.obj");
	//auto v = f.get_vertices();
	auto t =lighting_file();
	t.init("sphere.obj");
	t.Paint();
}