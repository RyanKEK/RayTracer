#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vec3d.h"
#include "Ray.h"
#include "Shape.h"

inline bool intersect_plane(const Vec3d& n, const Vec3d& p0, const Vec3d& l0, const Vec3d& l, double& t)
{
	double denom = n.dot(l);
	if (denom > 1e-6) {
		Vec3d p0l0 = p0 - l0;
		t = n.dot(p0l0) / denom;
		return (t >= 0);
	}

	return false;
}

inline bool rayTriangleIntersect(
	const Vec3d& orig, const Vec3d& dir,
	const Vec3d& v0, const Vec3d& v1, const Vec3d& v2,
	double& t)
{
	// compute plane's normal
	Vec3d v0v1 = v1 - v0;
	Vec3d v0v2 = v2 - v0;
	// no need to normalize
	Vec3d N = v0v1.cross(v0v2); // N 
	float area2 = N.length();

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float NdotRayDirection = N.dot(dir);
	if (fabs(NdotRayDirection) < 0.000000000000000001) // almost 0 
		return false; // they are parallel so they don't intersect ! 

	// compute d parameter using equation 2
	float d = N.dot(v0);

	// compute t (equation 3)
	t = (N.dot(orig) + d) / NdotRayDirection;
	// check if the triangle is in behind the ray
	if (t < 0) return false; // the triangle is behind 

	// compute the intersection point using equation 1
	Vec3d P = orig + t * dir;

	// Step 2: inside-outside test
	Vec3d C; // vector perpendicular to triangle's plane 

	// edge 0
	Vec3d edge0 = v1 - v0;
	Vec3d vp0 = P - v0;
	C = edge0.cross(vp0);
	if (N.dot(C) < 0) return false; // P is on the right side 

	// edge 1
	Vec3d edge1 = v2 - v1;
	Vec3d vp1 = P - v1;
	C = edge1.cross(vp1);
	if (N.dot(C) < 0)  return false; // P is on the right side 

	// edge 2
	Vec3d edge2 = v0 - v2;
	Vec3d vp2 = P - v2;
	C = edge2.cross(vp2);
	if (N.dot(C) < 0) return false; // P is on the right side; 

	return true; // this ray hits the triangle 
}

class Triangle
{
public:

	Vec3d p1;
	Vec3d p2;
	Vec3d p3;

	Triangle(Vec3d p1, Vec3d p2, Vec3d p3) : p1(p1), p2(p2), p3(p3) {}
	~Triangle() {}
	bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec)
	{
		double t;
		Vec3d AB = (p2 - p1);
		Vec3d BC = (p3 - p2);
		Vec3d CA = (p1 - p3);
		//std::cout << "A: " << p1 << std::endl;
		//std::cout << "B: " << p2 << std::endl;
		//std::cout << "C: " << p3 << std::endl;
		//std::cout << "AB" << AB << std::endl;
		//std::cout << "BC" << BC << std::endl;
		//std::cout << "CA" << CA << std::endl;
		if (intersect_plane(AB.cross(BC).normalized(), p1, ray.origin, ray.direction, t))
		{
			Vec3d point = ray.origin + ray.direction * t;
			//std::cout << point << std::endl;
			//Vec3d point = Vec3d(0.7, 0.7, -1);

			//std::cout << "asd\n";
			//std::cout << point << std::endl;;
			Vec3d AP = (point - p1);
			Vec3d BP = (point - p2);
			Vec3d CP = (point - p3);
			//std::cout << "AB" << AB << std::endl;
			//std::cout << "BC" << BC << std::endl;
			//std::cout << "CA" << CA << std::endl;

			Vec3d norm = AB.cross(BC).normalized();
			//std::cout << "norm: " << norm << std::endl;
			//std::cout << norm.dot(BC.cross(BP)) << std::endl;
			//std::cout << norm.dot(CA.cross(CP)) << std::endl;
			//std::cout << norm.dot(AB.cross(AP)) << std::endl;
			//std::cout << "norm: " << norm << std::endl;
			//std::cout << "cross ABAP: " << AB.cross(AP) << std::endl;
			//std::cout << "dot: " << norm.dot(AB.cross(AP)) << std::endl;
			//std::cout << "cross BCBP: " << BC.cross(BP) << std::endl;
			//std::cout << "dot: " << norm.dot(BC.cross(BP)) << std::endl;
			//std::cout << "cross CACP: " << CA.cross(CP) << std::endl;
			//std::cout << "dot: " << norm.dot(CA.cross(CP)) << std::endl;


			if (norm.dot(AB.cross(AP)) > 0 && norm.dot(BC.cross(BP)) > 0
				&& norm.dot(CA.cross(CP)) > 0 && point.length() < t_max && point.length() > t_min)
			{
				rec.t = point.length();
				rec.hit_pos = point;
				rec.normal = norm;
				return true;
			}
			norm  *= -1;
			if (norm.dot(AB.cross(AP)) > 0 && norm.dot(BC.cross(BP)) > 0
				&& norm.dot(CA.cross(CP)) > 0 && point.length() < t_max && point.length() > t_min)
			{
				//std::cout << "asddas\n";
				rec.t = point.length();
				rec.hit_pos = point;
				rec.normal = norm;
				return true;
			}

		}
		//if (rayTriangleIntersect(ray.origin, ray.direction, p1, p2, p3, t))
		//{
		//	if (t < t_max && t > t_min)
		//	{
		//		Vec3d point = ray.origin + ray.direction * t;
		//		Vec3d p1p2 = p2 - p1;
		//		Vec3d p2p3 = p3 - p1;
		//		// no need to normalize
		//		Vec3d norm = p1p2.cross(p2p3); // N 
		//		rec.t = t;
		//		rec.hit_pos = point;
		//		rec.normal = norm;
		//		return true;
		//	}
		//}
		return false;
	}
};

//class Triangle
//{
//public:
//
//	Vec3d p1;
//	Vec3d p2;
//	Vec3d p3;
//
//	Triangle(Vec3d p1, Vec3d p2, Vec3d p3) : p1(p1), p2(p2), p3(p3) {}
//	~Triangle() {}
//	bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec)
//	{
//		double t;
//		if (rayTriangleIntersect(ray.origin, ray.direction, p1, p2, p3, t))
//		{
//			if (t < t_max && t > t_min)
//			{
//				Vec3d point = ray.origin + ray.direction * t;
//				Vec3d p1p2 = p2 - p1;
//				Vec3d p2p3 = p3 - p1;
//				// no need to normalize
//				Vec3d norm = p1p2.cross(p2p3); // N 
//				rec.t = t;
//				rec.hit_pos = point;
//				rec.normal = norm;
//				return true;
//			}
//		}
//		return false;
//	}
//};

#endif // !TRIANGLE_H
