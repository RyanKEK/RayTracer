#include "Model.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>

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

static double maxf(double a, double b, double c)
{
	return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

static bool sort_by_z(Triangle tri1, Triangle tri2)
{
	return (tri1.p1.z + tri1.p2.z + tri1.p3.z > tri2.p1.z + tri2.p2.z + tri2.p3.z);
}

Model::Model(Vec3d pos, std::string file_path, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	char type;
	double x, y, z,
		maxx = std::numeric_limits<double>::min(),
		minx = std::numeric_limits<double>::max(),
		maxy = std::numeric_limits<double>::min(),
		miny = std::numeric_limits<double>::max(),
		maxz = std::numeric_limits<double>::min(),
		minz = std::numeric_limits<double>::max();
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
			x = std::stod(str, &sz) / 4;
			if (x > maxx)
				maxx = x;
			else if (x < minx)
				minx = x;
			str = str.substr(str.find_first_of(' ') + 1);
			y = std::stod(str, &sz) / 4;
			if (y > maxy)
				maxy = y;
			else if (y < miny)
				miny = y;
			str = str.substr(str.find_first_of(' ') + 1);
			z = std::stod(str, &sz) / 4;
			if (z > maxz)
				maxz = z;
			else if (z < minz)
				minz = z;
			center = Vec3d((maxx + minx) / 2, (maxy + miny) / 2, (maxz + minz) / 2);
			//this->encircling_shpere_radius = maxf(fabs(maxx - minx), fabs(maxy - miny), fabs(maxz - minz)) / 2;
			this->encircling_shpere_radius = Vec3d(maxx - minx, maxy - miny, maxz-minz).length() / 2;
			vertexes.push_back(Vec3d(x, y, -z));
			std::cout << Vec3d(x, y, -z) << std::endl;
			outfile  << "v " << std::fixed << std::setprecision(6) << x  << " " << std::setprecision(6) << y  << " " << std::setprecision(6) << z << std::endl;
		}
		else if (str[0] == 'f')
		{
			outfile << "f ";
			str = str.substr(str.find_first_of(' ') + 1);
			x = std::stod(str, &sz) - 1;
			outfile << (int)x << " ";
			str = str.substr(str.find_first_of(' ') + 1);
			y = std::stod(str, &sz) - 1;
			outfile << (int)y << " ";
			str = str.substr(str.find_first_of(' ') + 1);
			z = std::stod(str, &sz) - 1;
			outfile << (int)z << " \n";
			this->triangles.push_back(Triangle(vertexes[x] + pos, vertexes[y] + pos, vertexes[z] + pos));
		}
	}
	std::sort(this->triangles.begin(), this->triangles.end(), sort_by_z);
	std::cout << file_path.substr(file_path.find_last_of('\\') + 1) <<" loaded\n";
	infile.close();
}

Model::~Model()
{

}

static bool hit_encircling_shpere(const t_ray& ray, double radius, const Vec3d& pos)
{
	Vec3d oc = ray.origin - pos;
	double a = ray.direction.length_squared();
	double half_b = oc.dot(ray.direction);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant > 0.0)
		return true;
	return false;
}

bool Model::hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
{
	bool intersection = false;
	double closest_so_far = t_max;
	hit_record temp_rec;
	int i = 0;

	if (hit_encircling_shpere(ray, this->encircling_shpere_radius, this->pos + this->center))
	{
		for (auto tri : triangles)
		{
			i++;
			if (tri.hit(ray, t_min, closest_so_far, temp_rec)) {
				//std::cout << "asdsad\n";

				closest_so_far = temp_rec.t;

				//std::cout << closest_so_far << std::endl;
				//std::cout << rec.mat_ptr;
				rec = temp_rec;


				intersection = true;
				break;
			}
		}
	}
	if (intersection) {
		rec.mat_ptr = mat_ptr;
		//std::cout << rec.mat_ptr << std::endl;
	}
	return intersection;
}