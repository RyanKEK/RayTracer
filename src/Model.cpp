#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <future>
#include "../include/Model.h"

Model::Model(Vec3d pos, const std::vector<Triangle>& triangles, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	this->triangles = triangles;
	for (auto &tri : this->triangles) {
		tri.p1 += pos;
		tri.p2 += pos;
		tri.p3 += pos;
	}
}

static double maxf(double a, double b, double c)
{
	return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

static double minf(double a, double b, double c)
{
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

static bool sort_by_z(Triangle tri1, Triangle tri2)
{
	return (tri1.p1.z + tri1.p2.z + tri1.p3.z > tri2.p1.z + tri2.p2.z + tri2.p3.z);
}

static double clamp(double n, double min, double max)
{
	if (n < min)
		return min;
	if (n > max)
		return max;
	return n;
}

bool rayBoxIntersect(t_ray ray, Vec3d lb, Vec3d rt)
{
	Vec3d dirfrac;
	dirfrac.x = 1.0f / ray.direction.x;
	dirfrac.y = 1.0f / ray.direction.y;
	dirfrac.z = 1.0f / ray.direction.z;
	float t1 = (lb.x - ray.origin.x) * dirfrac.x;
	float t2 = (rt.x - ray.origin.x) * dirfrac.x;
	float t3 = (lb.y - ray.origin.y) * dirfrac.y;
	float t4 = (rt.y - ray.origin.y) * dirfrac.y;
	float t5 = (lb.z - ray.origin.z) * dirfrac.z;
	float t6 = (rt.z - ray.origin.z) * dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0)
	{
		return false;
	}

	if (tmin > tmax)
	{
		return false;
	}


	return true;
}

Model::Model(Vec3d pos, std::string file_path, std::shared_ptr<Material> mat) : Shape(pos, mat)
{
	char type;
	double x, y, z,
		maxx = -std::numeric_limits<double>::max(),
		minx = std::numeric_limits<double>::max(),
		maxy = -std::numeric_limits<double>::max(),
		miny = std::numeric_limits<double>::max(),
		maxz = -std::numeric_limits<double>::max(),
		minz = std::numeric_limits<double>::max();
	std::ifstream infile("../RayTracer/Models/" + file_path);
	if (infile.good() == false)
	{
		std::cout << "Cannot open " << file_path << "\n";
		return;
	}
	std::vector<Vec3d> vertexes;
	std::vector<Tex> vt;
	std::vector<Vec3d> vn;
	std::string str;
	std::string::size_type sz;
	this->texture = IMG_Load("../RayTracer/Textures/Red.png");
	while (std::getline(infile, str))
	{
		if (str[0] == 'v' && str[1] == ' ')
		{
			str = str.substr(str.find_first_of(' ') + 1);
			x = std::stod(str, &sz);
			if (x > maxx)
				maxx = x;
			else if (x < minx)
				minx = x;
			str = str.substr(str.find_first_of(' ') + 1);
			y = std::stod(str, &sz);
			if (y > maxy)
				maxy = y;
			else if (y < miny)
				miny = y;
			str = str.substr(str.find_first_of(' ') + 1);
			z = std::stod(str, &sz);
			if (z > maxz)
				maxz = z;
			else if (z < minz)
				minz = z;
			center = Vec3d((maxx + minx) / 2, (maxy + miny) / 2, (maxz + minz) / 2);
			//this->encircling_shpere_radius = maxf(fabs(maxx - minx), fabs(maxy - miny), fabs(maxz - minz)) / 2;
			this->encircling_shpere_radius = Vec3d(maxx - minx, maxy - miny, maxz-minz).length() / 2;

			vertexes.push_back(Vec3d(-x, y, z));
			//std::cout << Vec3d(x, y, -z) << std::endl;
		}
		else if (str[0] == 'v' && str[1] == 't')
		{
			Tex tmp;
			str = str.substr(str.find_first_of(' ') + 1);
			//tmp.u = clamp(std::stod(str, &sz), 0, 1);
			tmp.u = std::stod(str, &sz);
			//if (tmp.u == 0)
			//	tmp.u += 0.5;
			str = str.substr(str.find_last_of(' ') + 1);
			//tmp.v = clamp(std::stod(str, &sz), 0, 1);
			tmp.v = std::stod(str, &sz);
			vt.push_back(tmp);
			//std::cout << tmp.u << " " << tmp.v << std::endl;
			
		}
		else if (str[0] == 'v' && str[1] == 'n')
		{
			Vec3d tmp;
			str = str.substr(str.find_first_of(' ') + 1);
			tmp.x = std::stod(str);
			str = str.substr(str.find_first_of(' ') + 1);
			tmp.y = -std::stod(str);
			str = str.substr(str.find_first_of(' ') + 1);
			tmp.z = -std::stod(str);
			vn.push_back(tmp);
		}
		else if (str[0] == 'f')
		{
			Vec3d p1Norm;
			Vec3d p2Norm;
			Vec3d p3Norm;
			std::vector<Tex> tmp;
			str = str.substr(str.find_first_of(' ') + 1);
			x = std::stod(str, &sz) - 1;
			str = str.substr(str.find_first_of('/') + 1);
			tmp.push_back(vt[std::stoi(str) - 1]);
			str = str.substr(str.find_first_of('/') + 1);
			p1Norm = vn[std::stoi(str) - 1];
			//std::cout << std::stoi(str) << " ";
			str = str.substr(str.find_first_of(' ') + 1);
			y = std::stod(str, &sz) - 1;
			str = str.substr(str.find_first_of('/') + 1);
			tmp.push_back(vt[std::stoi(str) - 1]);
			str = str.substr(str.find_first_of('/') + 1);
			p2Norm = vn[std::stoi(str) - 1];
			//std::cout << std::stoi(str) << " ";
			//std::cout << str.substr(2) << std::endl;
			//std::cout << str << std::endl;
			str = str.substr(str.find_first_of(' ') + 1);
			z = std::stod(str, &sz) - 1;
			str = str.substr(str.find_first_of('/') + 1);
			tmp.push_back(vt[std::stoi(str) - 1]);
			str = str.substr(str.find_first_of('/') + 1);
			p3Norm = vn[std::stoi(str) - 1];
			//std::cout << std::stoi(str) << std::endl;
			this->texture_coordinates.push_back(tmp);
			Triangle tmpTriangle = Triangle(vertexes[x] + pos, vertexes[y] + pos, vertexes[z] + pos, tmp);
			tmpTriangle.p1Norm = p1Norm;
			tmpTriangle.p2Norm = p2Norm;
			tmpTriangle.p3Norm = p3Norm;
			this->triangles.push_back(tmpTriangle);
		}
	}
	this->leftBottom = Vec3d(minx, miny, minz) + pos;
	this->rightTop = Vec3d(maxx, maxy, maxz) + pos;
	//std::sort(this->triangles.begin(), this->triangles.end(), sort_by_z);
	std::cout << file_path.substr(file_path.find_last_of('\\') + 1) <<" loaded\n";
	infile.close();
}

Model::~Model()
{

}

void Model::resize(double scale)
{
	
	double
		maxx = -std::numeric_limits<double>::max(),
		minx = std::numeric_limits<double>::max(),
		maxy = -std::numeric_limits<double>::max(),
		miny = std::numeric_limits<double>::max(),
		maxz = -std::numeric_limits<double>::max(),
		minz = std::numeric_limits<double>::max();
	for (auto& tri : this->triangles)
	{
		tri.p1 -= this->pos;
		tri.p2 -= this->pos;
		tri.p3 -= this->pos;
		tri.p1 *= scale;
		tri.p2 *= scale;
		tri.p3 *= scale;
		tri.p1 += this->pos;
		tri.p2 += this->pos;
		tri.p3 += this->pos;
		tri.norm = (tri.p2 - tri.p1).cross(tri.p3 - tri.p2);
		tri.p1p2 = tri.p2 - tri.p1;
		tri.p1p3 = tri.p3 - tri.p1;

		if (tri.p1.x > maxx)
			maxx = tri.p1.x;
		else if (tri.p1.x < minx)
			minx = tri.p1.x;
		if (tri.p2.x > maxx)
			maxx = tri.p2.x;
		else if (tri.p2.x < minx)
			minx = tri.p2.x;
		if (tri.p3.x > maxx)
			maxx = tri.p3.x;
		else if (tri.p3.x < minx)
			minx = tri.p3.x;
		if (tri.p1.y > maxy)
			maxy = tri.p1.y;
		else if (tri.p1.y < miny)
			miny = tri.p1.y;
		if (tri.p2.y > maxy)
			maxy = tri.p2.y;
		else if (tri.p2.y < miny)
			miny = tri.p2.y;
		if (tri.p3.y > maxy)
			maxy = tri.p3.y;
		else if (tri.p3.y < miny)
			miny = tri.p3.y;
		if (tri.p1.z > maxz)
			maxz = tri.p1.z;
		else if (tri.p1.z < minz)
			minz = tri.p1.z;
		if (tri.p2.z > maxz)
			maxz = tri.p2.z;
		else if (tri.p2.z < minz)
			minz = tri.p2.z;
		if (tri.p3.z > maxz)
			maxz = tri.p3.z;
		else if (tri.p3.z < minz)
			minz = tri.p3.z;
	}
	this->encircling_shpere_radius *= scale;
	this->leftBottom = Vec3d(minx, miny, minz);
	this->rightTop = Vec3d(maxx, maxy, maxz);
}

void Model::rotateX(double angle)
{
	angle *= 0.0174533;
	for (auto& tri : this->triangles)
	{
		tri.p1 -= this->pos;
		tri.p2 -= this->pos;
		tri.p3 -= this->pos;
		tri.p1 = { tri.p1.x, tri.p1.y * cos(angle) - tri.p1.z * sin(angle), tri.p1.y * sin(angle) + tri.p1.z * cos(angle) };
		tri.p2 = { tri.p2.x, tri.p2.y * cos(angle) - tri.p2.z * sin(angle), tri.p2.y * sin(angle) + tri.p2.z * cos(angle) };
		tri.p3 = { tri.p3.x, tri.p3.y * cos(angle) - tri.p3.z * sin(angle), tri.p3.y * sin(angle) + tri.p3.z * cos(angle) };
		tri.p1 += this->pos;
		tri.p2 += this->pos;
		tri.p3 += this->pos;
	}
}

void Model::rotateY(double angle)
{
	angle *= 0.0174533;
	for (auto& tri : this->triangles)
	{
		tri.p1 -= this->pos;
		tri.p2 -= this->pos;
		tri.p3 -= this->pos;
		tri.p1 = { tri.p1.x * cos(angle) + tri.p1.z * sin(angle), tri.p1.y, -tri.p1.x * sin(angle) + tri.p1.z * cos(angle) };
		tri.p2 = { tri.p2.x * cos(angle) + tri.p2.z * sin(angle), tri.p2.y, -tri.p2.x * sin(angle) + tri.p2.z * cos(angle) };
		tri.p3 = { tri.p3.x * cos(angle) + tri.p3.z * sin(angle), tri.p3.y, -tri.p3.x * sin(angle) + tri.p3.z * cos(angle) };
		tri.p1 += this->pos;
		tri.p2 += this->pos;
		tri.p3 += this->pos;
	}
}

void Model::rotateZ(double angle)
{
	angle *= 0.0174533;
	for (auto& tri : this->triangles)
	{
		tri.p1 -= this->pos;
		tri.p2 -= this->pos;
		tri.p3 -= this->pos;
		tri.p1 = { tri.p1.x * cos(angle) - tri.p1.y * sin(angle), tri.p1.x * sin(angle) + tri.p1.y * cos(angle),  tri.p1.z };
		tri.p2 = { tri.p2.x * cos(angle) - tri.p2.y * sin(angle), tri.p2.x * sin(angle) + tri.p2.y * cos(angle),  tri.p2.z };
		tri.p3 = { tri.p3.x * cos(angle) - tri.p3.y * sin(angle), tri.p3.x * sin(angle) + tri.p3.y * cos(angle),  tri.p3.z };
		tri.p1 += this->pos;
		tri.p2 += this->pos;
		tri.p3 += this->pos;
	}
}

static Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

bool Model::hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const
{
	bool intersection = false;
	double closest_so_far = t_max;
	hit_record temp_rec;
	int i = 0;
	//SDL_Surface* image;
	//
	//image = IMG_Load("ComeHere.jpg");
	//double distance;
	//std::cout << this->box[4] << " " << this->box[0] << std::endl;
	//if (rayBoxIntersect(ray, this->box[0], this->box[1], distance))
	//{
	//	//std::cout << "hit";
	//	rec.hit_color = { 0.8, 0.5, 0.9 };
	//	rec.t = distance;
	//	intersection = true;
	//}
	//if (hit_encircling_shpere(ray, this->encircling_shpere_radius, this->pos + this->center))
	//{
	//std::vector<std::future<void>> futures;
	if (rayBoxIntersect(ray, this->leftBottom, this->rightTop))
	{
		//Timer timer;
		for (auto& tri : triangles)
		{
			i++;
			if (tri.hit(ray, t_min, closest_so_far, temp_rec))
			{
				
				//Vec3d norm = (tri.p2 - tri.p1).cross(tri.p3 - tri.p2);
				double area = tri.norm.length() / 2;
				
				Vec3d edge1 = tri.p3 - tri.p2;
				Vec3d vp1 = temp_rec.hit_pos - tri.p2;
				Vec3d C = edge1.cross(vp1);
				double u = (C.length() / 2) / area;
				
				Vec3d edge2 = tri.p1 - tri.p3;
				Vec3d vp2 = temp_rec.hit_pos - tri.p3;
				C = edge2.cross(vp2);
				double v = (C.length() / 2) / area;
				
				i -= 1;
				Tex tmp = { tri.texture_coordinates[0].u * u + tri.texture_coordinates[1].u * v + tri.texture_coordinates[2].u * (1.0 - u - v),
							tri.texture_coordinates[0].v * u + tri.texture_coordinates[1].v * v + tri.texture_coordinates[2].v * (1.0 - u - v) };

				Uint32 color_value = getpixel(this->texture, this->texture->w * tmp.u, this->texture->h - (this->texture->h * tmp.v));
				Uint8 r, g, b, a;
				SDL_GetRGBA(color_value, this->texture->format, &r, &g, &b, &a);
				temp_rec.hit_color = Vec3d(r / 255.0, g / 255.0, b / 255.0);
				closest_so_far = temp_rec.t;
				rec = temp_rec;
				rec.normal = (tri.p1Norm * u + tri.p2Norm * v + tri.p3Norm * (1 - u - v)).normalized();
				intersection = true;
				//break;
			}
		}
		//std::cout << "Model intersection took " << timer.diffMS() << std::endl;
	}
	if (intersection)
	{
		rec.mat_ptr = mat_ptr;
		//std::cout << rec.mat_ptr << std::endl;
	}
	//std::cout << "Model intersection took " << timer.diffMS() << std::endl;
	return intersection;
}