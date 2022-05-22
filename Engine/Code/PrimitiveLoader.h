#pragma once
#include "platform.h"

struct App;
class Sphere;
class Plane;

namespace PrimitiveLoader
{
	u32 LoadSphere(App* app);

	u32 LoadPlane(App* app);

	u32 LoadCube();

};
