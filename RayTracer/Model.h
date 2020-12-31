#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Triangle.h"

class Model : public Shape
{
public:

	std::vector<Triangle> triangles;


	Model(Vec3d pos, const std::vector<Triangle>& triangles, std::shared_ptr<Material> mat);
	Model(Vec3d pos, std::string file_path, std::shared_ptr<Material> mat);
	~Model();

	virtual bool hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const override;
};

#endif // !TRIANGLE_H
