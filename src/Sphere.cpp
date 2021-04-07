#include "../include/Sphere.h"
#include <fstream>

std::ofstream file;

Sphere::Sphere() : Shape()
{
	this->radius = 1.0;
}

Sphere::Sphere(Vec3d pos, double radius, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	this->radius = radius;
}

Sphere::~Sphere()
{
}

bool Sphere::hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
{
	//file.open("CurrOutput.txt", std::ofstream::app);
	//std::cout << ray.direction;
	Vec3d oc = ray.origin - pos;
	double a = ray.direction.length_squared();
	double half_b = oc.dot(ray.direction);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	//file << "ray.direction:" << ray.direction << std::endl;
	//file << "oc: " << oc << std::endl;
	//file << "a: " << a << std::endl;
	//file << "half_b: " << half_b << std::endl;
	//file << "c: " << c << std::endl;
	//file << "discriminant: " << discriminant << std::endl;
	//file.close();
	if (discriminant > 0.0)
	{
		double root = sqrt(discriminant);

		double temp = (-half_b - root) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.hit_pos = ray.origin + rec.t * ray.direction;
			rec.normal = (rec.hit_pos - pos) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.hit_pos = ray.origin + rec.t * ray.direction;
			rec.normal = (rec.hit_pos - pos) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}

	}
	return false;
}