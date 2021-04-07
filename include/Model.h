#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Triangle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <array>

class Model : public Shape
{
public:

	std::vector<Triangle>			triangles;
	std::vector<std::vector<Tex>>	texture_coordinates;
	Vec3d							leftBottom;
	Vec3d							rightTop;
	double							encircling_shpere_radius;
	Vec3d							center;
	SDL_Surface*					texture;

	Model(Vec3d pos, const std::vector<Triangle>& triangles, std::shared_ptr<Material> mat);
	Model(Vec3d pos, std::string file_path, std::shared_ptr<Material> mat);
	~Model();

	void resize(double scale);
	void rotateX(double angle);
	void rotateY(double angle);
	void rotateZ(double angle);



	virtual bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const override;
};

#endif // !MODEL_H
