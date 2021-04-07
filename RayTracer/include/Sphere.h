#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Ray.h"
#include "Shape.h"

class Sphere : public Shape
{
public:
	double radius;

	Sphere();
	Sphere(Vec3d pos,  double radius, std::shared_ptr<Material> mat);
	~Sphere();

	virtual bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const override;
};

#endif // !SPHERE_H