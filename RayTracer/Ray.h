#pragma once
#ifndef RAY_H
#define RAY_H

#include "Vec3d.h"

struct t_ray
{
	Vec3d origin;
	Vec3d direction;
};

#endif // !RAY_H