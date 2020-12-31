#pragma once
#ifndef SHAPE_H
#define SHAPE_H


#include "Vec3d.h"
#include "Ray.h"
#include <memory>

class Material;

struct hit_record
{
	Vec3d hit_pos;
	Vec3d normal;
	double t;
	std::shared_ptr<Material> mat_ptr;
};

class Shape
{
	
public:
	Vec3d pos;
	std::shared_ptr<Material> mat_ptr;

	Shape();
	Shape(Vec3d pos, std::shared_ptr<Material> mat);
	~Shape();

	virtual bool hit(const t_ray &ray, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif // !SHAPE_H
