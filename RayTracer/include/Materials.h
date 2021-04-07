#ifndef MATERIAL_H
#define MATERIAL_H

#include "main.h"

struct hit_record;

class Material
{
public:
	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& color, t_ray& scattered) const = 0;
	virtual ~Material() {};
};

inline Vec3d reflect(const Vec3d& vec, const Vec3d& surface_normal)
{
	return (vec - 2 * vec.dot(surface_normal) * surface_normal);
}

class Metal : public Material
{
public:

	Vec3d color;
	double fuzz;

	Metal() : color(Vec3d(0, 0, 0)), fuzz(0) {}
	Metal(const Vec3d& new_color, double f = 0) : color(new_color), fuzz(f) {}
	~Metal() {};

	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& attenuation, t_ray& scattered) const override
	{
		Vec3d reflected = reflect(ray.direction.normalized(), rec.normal.normalized());
		//reflected *= -1;
		scattered = t_ray{ rec.hit_pos, reflected + fuzz * random_in_unit_sphere()};
		attenuation = color;
		//std::cout << color << std::endl;
		//return (scattered.direction.dot(rec.normal) > 0);
		return true;
	}

};

class Lambertian : public Material
{
public:
	Vec3d color;

	Lambertian() : color(Vec3d(0, 0, 0)){}
	Lambertian(const Vec3d& new_color) : color(new_color){}
	~Lambertian() {};

	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& attenuation, t_ray& scattered) const override
	{
		Vec3d scatter_direction = rec.normal + random_unit_vector();
		//std::cout << random_unit_vector().length() << std::endl;
		//std::cout << color << std::endl;
		//scatter_direction = { 0, -1 ,0 };
		scattered = t_ray{ rec.hit_pos, scatter_direction };
		attenuation = color;
		return (true);
	}

};

class PlainTexture : public Material
{
public:

	PlainTexture() {}
	~PlainTexture() {};

	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& attenuation, t_ray& scattered) const override
	{
		attenuation = rec.hit_color;
		return false;
	}

};

class MetalTexture : public Material
{
public:

	double fuzz;

	MetalTexture() : fuzz(0) {}
	MetalTexture(double f) : fuzz(f) {}
	~MetalTexture() {};

	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& attenuation, t_ray& scattered) const override
	{
		Vec3d reflected = reflect(ray.direction.normalized(), rec.normal);
		scattered = t_ray{ rec.hit_pos, reflected + fuzz * random_in_unit_sphere() };
		//attenuation = rec.hit_color;
		//attenuation.x = (static_cast<int>(rec.hit_pos.x) % 255) / 255.0;
		//attenuation.y = (static_cast<int>(rec.hit_pos.y) % 255) / 255.0;
		//attenuation.z = (static_cast<int>(rec.hit_pos.z) % 255) / 255.0;
		double intPart;
		attenuation.x = 1 * ((int)(abs(floor(rec.hit_pos.x))) + (int)(abs(floor(rec.hit_pos.z)))) % 2;
		attenuation.y = 1 * ((int)(abs(floor(rec.hit_pos.x))) + (int)(abs(floor(rec.hit_pos.z)))) % 2;
		attenuation.z = 1 * ((int)(abs(floor(rec.hit_pos.x))) + (int)(abs(floor(rec.hit_pos.z)))) % 2;
		//attenuation.z = modf(rec.hit_pos.z, &intPart);
		//attenuation.z = 0.5;
		/*attenuation  = { static_cast<int>(rec.hit_pos.x) % 255, static_cast<int>(rec.hit_pos.y) % 255, static_cast<int>(rec.hit_pos.x) % 255 };*/
		//std::cout << color << std::endl;
		//return (scattered.direction.dot(rec.normal) > 0);
		return false;
	}

};

class LambertianTexture : public Material
{
public:
	Vec3d color;

	LambertianTexture() : color(Vec3d(0, 0, 0)) {}
	LambertianTexture(const Vec3d& new_color) : color(new_color) {}
	~LambertianTexture() {};

	virtual bool scatter(const t_ray& ray, const hit_record& rec, Vec3d& attenuation, t_ray& scattered) const override
	{
		Vec3d scatter_direction = rec.normal + random_unit_vector();
		//std::cout << color << std::endl;
		scattered = t_ray{ rec.hit_pos, scatter_direction };
		attenuation = rec.hit_color;
		return (true);
	}

};


#endif