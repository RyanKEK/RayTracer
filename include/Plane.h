#pragma once
#include "Shape.h"

class Plane : public Shape
{
public:
	Vec3d norm;

	Plane() = default;
	Plane(Vec3d pos, Vec3d norm, std::shared_ptr<Material> mat) : Shape(pos, mat), norm(norm) {}

	bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const override
	{
		double denom = norm.dot(ray.direction);
		double t;
		if (denom > 1e-6)
		{
			Vec3d vecToPoint = pos - ray.origin;
			t = vecToPoint.dot(norm) / denom;
			if (t >= 0 && t < t_max && t > t_min)
			{
				rec.t = t;
				rec.hit_pos = ray.origin + rec.t * ray.direction;
				rec.normal = norm;
				rec.mat_ptr = mat_ptr;
				return true;
			}
			return false;
		}
		Vec3d norm = this->norm * -1;
		denom = norm.dot(ray.direction);
		if (denom > 1e-6)
		{
			Vec3d vecToPoint = pos - ray.origin;
			t = vecToPoint.dot(norm) / denom;
			if (t >= 0 && t < t_max && t > t_min)
			{
				rec.t = t;
				rec.hit_pos = ray.origin + rec.t * ray.direction;
				rec.normal = norm;
				rec.mat_ptr = mat_ptr;
				return true;
			}
			return false;
		}

		return false;
	}
};