#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vec3d.h"
#include "Ray.h"
#include "Shape.h"
#include "Timer.h"

struct Tex
{
	double u;
	double v;
};

inline bool intersect_plane(const Vec3d& norm, const Vec3d& point, const Vec3d& origin, const Vec3d& direction, double& t)
{
	double denom = norm.dot(direction);
	if (denom > 1e-6) {
		Vec3d p0l0 = point - origin;
		t = norm.dot(p0l0) / denom;
		return (t >= 0);
	}

	return false;
}

class Triangle
{
public:

	Vec3d p1;
	Vec3d p2;
	Vec3d p3;
	Vec3d p1Norm;
	Vec3d p2Norm;
	Vec3d p3Norm;
	Vec3d p1p2;
	Vec3d p1p3;
	Vec3d norm;
	std::vector<Tex> texture_coordinates;

	Triangle(Vec3d p1, Vec3d p2, Vec3d p3, std::vector<Tex> texture_coordinates)
		: p1(p1), p2(p2), p3(p3), p1p2(p2 - p1), p1p3(p3 - p1), norm((p2 - p1).cross(p3 - p2)), texture_coordinates(texture_coordinates){}
	~Triangle() {}
	//bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
	//{
	//	//Timer timer;
	//	double t;
	//	//Vec3d AB = (p2 - p1);
	//	//Vec3d BC = (p3 - p2);
	//	//Vec3d CA = (p1 - p3);
	//	if (intersect_plane(this->norm, p1, ray.origin, ray.direction, t))
	//	{
	//		Vec3d point = ray.origin + ray.direction * t;
	//		Vec3d AP = (point - p1);
	//		Vec3d BP = (point - p2);
	//		Vec3d CP = (point - p3);

	//		Vec3d norm = this->norm.normalized();
	//		//double rayLength = point.length();
	//		if (t < t_max && t > t_min
	//			&& norm.dot(AB.cross(AP)) > 0
	//			&& norm.dot(BC.cross(BP)) > 0
	//			&& norm.dot(CA.cross(CP)) > 0)
	//		{
	//			rec.t = t;
	//			rec.hit_pos = point;
	//			rec.normal = norm;
	//			//std::cout << "ray.hit took " << timer.diffMS() << std::endl;
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
	{
		//std::cout << this->p1p2 << " " << p1p2 << std::endl;
		//std::cout << this->p1p3 << " " << p1p3 << std::endl;
		Vec3d pvec = ray.direction.cross(p1p3);
		double det = p1p2.dot(pvec);
		if (fabs(det) < 0.000001)
			return false;

		double invDet = 1 / det;
		Vec3d tvec = ray.origin - p1;
		double u = tvec.dot(pvec) * invDet;
		if (u < 0 || u > 1)
			return false;

		Vec3d qvec = tvec.cross(p1p2);
		double v = ray.direction.dot(qvec) * invDet;
		if (v < 0 || u + v > 1)
			return false;

		double t = p1p3.dot(qvec) * invDet;
		if (t > t_max || t < t_min)
			return false;
		rec.t = t;
		rec.hit_pos = ray.origin + ray.direction * t;
		rec.normal = norm;


		return true;
	}
};

#endif // !TRIANGLE_H
