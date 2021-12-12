// Рисует треугольник, цвет которого можно менять с клавиатуры
using namespace std;
#include <iostream>

#include "key.h"
#include "triangle.h"
#include "gradient.h"
#include "cube.h"
#include "pentagon.h"
#include "yellow_cube.h"
#include "textures_color.h"
#include "ellipse.h"
#include "textures_double.h"
#include "lighting_Phong.h"

int main()
{
	auto t = lighting_Phong();//textures_double();//textures_color();
	t.Paint();
}