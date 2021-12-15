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
#include "lighting_Toon.h"
#include "lighting_Minnaert.h"

int main()
{
	//auto f = figure_model("model.obj");
	//auto v = f.get_vertices();
	auto t =lighting_file();
	t.init("model.obj");
	t.Paint();
	auto toon = lighting_Toon();
	toon.init("model.obj");
	toon.Paint();
	auto minnaert = lighting_minnaert();
	minnaert.init("model.obj");
	minnaert.Paint();
}