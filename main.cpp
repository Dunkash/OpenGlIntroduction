// Рисует треугольник, цвет которого можно менять с клавиатуры
using namespace std;
#include <iostream>

#include "key.h"
#include "triangle.h"
#include "gradient.h"
#include "cube.h"
#include "pentagon.h"
#include "yellow_cube.h"

int main()
{
	auto t = yellowcube();
	t.Paint();
	auto tt = pentagon();
	tt.Paint();
}