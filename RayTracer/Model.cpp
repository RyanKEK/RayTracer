#include "Model.h"
#include <fstream>
#include <string>
#include <iomanip>


Model::Model(Vec3d pos, const std::vector<Triangle>& triangles, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	this->triangles = triangles;
	for (auto &tri : this->triangles) {
		tri.p1 += pos;
		tri.p2 += pos;
		tri.p3 += pos;
	}
	//std::cout << this->triangles[0].p1 << std::endl;
	//std::cout << this->triangles[0].p2 << std::endl;
	//std::cout << this->triangles[0].p3 << std::endl;
}

Model::Model(Vec3d pos, std::string file_path, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	char type;
	double a, b, c;
	std::ifstream infile(file_path);
	std::ofstream outfile;
	outfile.open("MyModel.obj");
	std::vector<Vec3d> vertexes;
	std::cout << this->mat_ptr << std::endl;
	std::string str;
	std::string::size_type sz;
	while (std::getline(infile, str))
	{
		if (str[0] == 'v' && str[1] == ' ')
		{
			str = str.substr(str.find_first_of(' ') + 1);
			a = std::stod(str, &sz);
			str = str.substr(str.find_first_of(' ') + 1);
			b = std::stod(str, &sz);
			str = str.substr(str.find_first_of(' ') + 1);
			c = std::stod(str, &sz);
			vertexes.push_back(Vec3d(a / 4, b / 4, -c / 4));
			outfile  << "v " << std::fixed << std::setprecision(6) << a  << " " << std::setprecision(6) << b  << " " << std::setprecision(6) << c << std::endl;
		}
		else if (str[0] == 'f')
		{
			outfile << "f ";
			str = str.substr(str.find_first_of(' ') + 1);
			a = std::stod(str, &sz);
			outfile << (int)a << " ";
			str = str.substr(str.find_first_of(' ') + 1);
			b = std::stod(str, &sz);
			outfile << (int)b << " ";
			str = str.substr(str.find_first_of(' ') + 1);
			c = std::stod(str, &sz);
			outfile << (int)c << " \n";
			this->triangles.push_back(Triangle(vertexes[a] + pos, vertexes[b] + pos, vertexes[c] + pos));
			
		}
	}
	std::cout << "Loaded\n";
	infile.close();
}

Model::~Model()
{

}

bool Model::hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
{
	bool intersection = false;
	double closest_so_far = t_max;
	hit_record temp_rec;

	for (auto tri : triangles)
	{
		if (tri.hit(ray, t_min, closest_so_far, temp_rec)) {
			//std::cout << "asdsad\n";

			closest_so_far = temp_rec.t;
			//std::cout << closest_so_far << std::endl;
			//std::cout << rec.mat_ptr;
			rec = temp_rec;
			

			intersection = true;
		}
	}
	if (intersection) {
		rec.mat_ptr = mat_ptr;
		//std::cout << rec.mat_ptr << std::endl;
	}
	return intersection;
}