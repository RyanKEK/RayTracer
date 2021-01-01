#include "main.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <chrono>
#include <future>
#include "Vec3d.h"
#include "Ray.h"
#include "Sphere.h"
#include "ShapeContainer.h"
#include "Materials.h"
#include "Model.h"
#define ANTIALIASING

Vec3d WHITE =  Vec3d(1, 1, 1);
Vec3d RED = Vec3d(1, 0, 0);
Vec3d GREEN = Vec3d(0, 1, 0);
Vec3d BLUE = Vec3d(0, 0, 1);
Vec3d BLACK = Vec3d(0, 0, 0);


double cornerZ = -1.0;
SDL_Surface* image;
Vec3d default_sphere = { 0, 0, -4 };
double sphereR = 0.5;
double cameraZ = -1.0;


Vec3d lightSource = Vec3d(100, 100, -1);

std::vector<Vec3d> shapes;

int RenderW = RENDERW;
int RenderH = RENDERH;

int image_width = RenderW;

std::ofstream log_file;



void log(Vec3d vec)
{
	log_file << vec << std::endl;
}

void		put_pixel(SDL_Surface* surf, int x, int y, SDL_Color color)
{
	int* pix;

	pix = (int*)surf->pixels;
	pix[x + surf->w * y] = SDL_MapRGB(surf->format, color.r, color.g, color.b);
}

double clamp(double n, double min, double max)
{
	if (n < min)
		return min;
	if (n > max)
		return max;
	return n;
}

void put_antialiased_pixel(SDL_Surface* surf, int x, int y, Vec3d color, int samples_per_pixel)
{

	double scale = 1.0 / samples_per_pixel;
	double r = color.x;
	double g = color.y;
	double b = color.z;

	//r = sqrt(scale * r);
	//g = sqrt(scale * g);
	//b = sqrt(scale * b);
	r *= scale;
	g *= scale;
	b *= scale;
	put_pixel(surf, x, y, SDL_Color{ Uint8(clamp(r, 0.0, 0.999) * 256), Uint8(clamp(g, 0.0, 0.999) * 256), Uint8(clamp(b, 0.0, 0.999) * 256) });
}

int			init_sdl(t_sdl* sdl)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("%s\n", SDL_GetError());
		return (0);
	}
	sdl->win = SDL_CreateWindow("Win", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS);
	if (!sdl->win) {
		printf("%s\n", SDL_GetError());
		return (0);
	}
	sdl->ren = SDL_CreateRenderer(sdl->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	sdl->screen_surf = SDL_CreateRGBSurface(0, W, H, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	return (1);
}

double resolution_multiplier = 0.1;

int			event_manager(t_sdl* sdl, ShapeContainer& scene)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			return (0);
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z)
			scene.objects[0]->pos.z += 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
			scene.objects[0]->pos.z -= 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT)
			default_sphere.x += 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT)
			default_sphere.x -= 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
			default_sphere.z += 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP)
			default_sphere.z -= 0.05;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
			sphereR += 0.5;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t)
			sphereR -= 0.5;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_l)
			lightSource.z -= 100.0;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k)
			lightSource.z += 100.0;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w) {
			resolution_multiplier += 0.1;
			RenderW = W * resolution_multiplier;
			RenderH = H * resolution_multiplier;
			std::cout << RenderW << std::endl;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
			resolution_multiplier -= 0.1;
			RenderW = W * resolution_multiplier;
			RenderH = H * resolution_multiplier;
		}

		else if (event.type == SDL_MOUSEWHEEL) {
			cameraZ += event.wheel.y;
			printf("%d\n", event.wheel.y);
		}
	}
	return (1);
}

void		render_surface(SDL_Renderer* ren, SDL_Surface* surf, SDL_Rect* src_rect, SDL_Rect* dst_rect)
{
	SDL_Texture* tex;

	tex = SDL_CreateTextureFromSurface(ren, surf);
	SDL_RenderCopy(ren, tex, src_rect, dst_rect);
	SDL_RenderPresent(ren);
	SDL_DestroyTexture(tex);
}

double		distance3d(Vec3d vec1, Vec3d vec2)
{
	return ((vec2 - vec1).length());
}

void		prinVec3d(Vec3d a)
{
	printf("{ %lf, %lf, %lf }\n", a.x, a.y, a.z);
}

std::string vec_to_string(Vec3d vec)
{
	return ("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + "]");

}

Vec3d		define_vec(Vec3d origin, Vec3d direction, double length)
{
	return (origin + direction * length);
}

//double			hit_plane(Vec3d point, Vec3d norm, t_ray ray)
//{
//	Vec3d oc = ray.origin - point;
//	double	del = dot(ray.direction, norm);
//	if (del != 0.0)
//		return (-dot(oc, norm) / del);
//	else
//		return (-1);
//}

double dot(Vec3d vec1, Vec3d vec2)
{
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

double			hit_sphere(Vec3d center, double radius, t_ray ray)
{
	Vec3d oc = ray.origin - center;
	double a = ray.direction.length_squared();
	double half_b = oc.dot(ray.direction);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return -1;
	else
		return ((-half_b - sqrt(discriminant)) / a);
}

bool intersect_plane_func(const Vec3d& n, const Vec3d& p0, const Vec3d& l0, const Vec3d& l, double& t)
{
	// assuming vectors are all normalized
	double denom = n.dot(l);
	if (denom > 1e-6) {
		Vec3d p0l0 = p0 - l0;
		t = n.dot(p0l0) / denom;
		return (t >= 0);
	}

	return false;
}

bool intersect_disk(const Vec3d& n, const Vec3d& p0, const Vec3d& l0, const Vec3d& l, double radius)
{
	double t = 0;

	if (intersect_plane_func(n, p0, l0, l, t))
	{
		Vec3d p = l0 + l * t;
		Vec3d v = p - p0;
		double d2 = v.dot(v);
		return (sqrt(d2) <= radius);
	}
	return false;
}

bool intersect_triangle_func(Vec3d tri_p1, Vec3d tri_p2, Vec3d tri_p3, Vec3d point)
{
	Vec3d AB = (tri_p2 - tri_p1);
	Vec3d BC = (tri_p3 - tri_p2);
	Vec3d CA = (tri_p1 - tri_p3);
	Vec3d AP = (point - tri_p1);
	Vec3d BP = (point - tri_p2);
	Vec3d CP = (point - tri_p3);
	Vec3d norm = AB.cross(BC).normalized();
	//std::cout << "Point: " << point << std::endl;
	//std::cout << AB.cross(AP).normalized() << std::endl;
	//std::cout << BC.cross(BP).normalized() << std::endl;
	//std::cout << CA.cross(CP).normalized() << std::endl;
	//if (AB.cross(AP).dot(BC.cross(BP)) > 0 && BC.cross(BP).dot(CA.cross(CP)) > 0 
	//	&& CA.cross(CP).dot(AB.cross(AP)) > 0)
	if (norm.dot(BC.cross(BP)) > 0 && norm.dot(CA.cross(CP)) > 0
		&& norm.dot(AB.cross(AP)) > 0)
		return true;
	return false;
}

Vec3d	ray_color(const t_ray& ray, const ShapeContainer& scene, int depth)
{
	double t;
	//int *pix = (int*)image->pixels;
	hit_record rec;

	if (depth <= 0) {
		//std::cout << "asd\n";
		return Vec3d(0, 0, 0);
	}
	////if (intersect_plane(AB.cross(BC).normalized(), A, ray.origin, ray.direction, t))
	////{

	////	//std::cout << "T: " << t << std::endl;
	////	Vec3d p = ray.origin + ray.direction * t;
	////	//std::cout << p << std::endl;
	////	if (intersect_triangle(A, B, C, p))
	////		return (Vec3d(0, 0, 255));
	////}
	////if (intersect_disk(AB.cross(BC).normalized(), AB.cross(BC).normalized(), ray.origin, ray.direction, 0.3))
	////{

	////	//std::cout << "T: " << t << std::endl;
	////	//std::cout << p << std::endl;
	////	//if (intersect_triangle(A, B, C, p))
	////	return (Vec3d(255, 0, 0));
	////}
	//	//return Vec3d(50, 0, 0);

	if (scene.hit(ray, 0.001, infinity, rec))
	{
		t_ray scattered;
		Vec3d attenuation;
		//std::cout << rec.mat_ptr << std::endl;
		if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
		{
			//std::cout << attenuation << std::endl;
			//double shadow = 1 + rec.normal.dot(lightSource.normalized());
			return (attenuation * ray_color(scattered, scene, depth - 1));
		}
		return (Vec3d(0, 0, 0));
	}
	Vec3d normalized_dir = ray.direction.normalized();
	t = 0.5 * (normalized_dir.y + 1.0);
	Vec3d color = (1.0 - t) * Vec3d(1.0, 1.0, 1.0) + t * Vec3d(0.5, 0.7, 1.0);
	//std::cout << "Color in func: " << color << std::endl;
	return (color);
}

void render_column(int xmin, int xmax, t_sdl& sdl, ShapeContainer& scene)
{
	std::chrono::time_point<std::chrono::system_clock> StartTime;
	std::chrono::time_point<std::chrono::system_clock> FinishTime;
	//std::cout << "Started rendering from " << xmin << " to " << xmax << std::endl;
	const auto aspect_ratio = (double)RENDERW / RENDERH;
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = Vec3d(0, 0, 0);
	auto horizontal = Vec3d(viewport_width, 0, 0);
	auto vertical = Vec3d(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3d(0, 0, focal_length);

	const int samples_per_pixel = 20;
	Vec3d color = { 0, 0, 0 };
	t_ray ray;

	int	nx = RenderW;
	int ny = RenderH;
	image_width = RenderW;
	int image_height = static_cast<int>(image_width / aspect_ratio);
	StartTime = std::chrono::system_clock::now();
	for (int y = 0; y <= ny; y++)
	{
		for (int x = xmin; x < xmax; x++)
		{
#ifdef ANTIALIASINGs
			color = { 0, 0, 0 };
			for (int i = 0; i < samples_per_pixel; i++)
			{
				double u = (x + random_double()) / ((double)image_width - 1);
				double v = (y + random_double()) / ((double)image_height - 1);
				ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
				color += ray_color(ray, scene, 50);
			}
			//log(color);
			put_antialiased_pixel(sdl.screen_surf, x, ny - y, color, samples_per_pixel);
#else
			double u = x / ((double)image_width - 1);
			double v = y / ((double)image_height - 1);
			ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
			color = ray_color(ray, scene, 50);
			put_pixel(sdl.screen_surf, x, ny - y, SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255), (Uint8)(color.z * 255) });
#endif // ANTIALIASING
		}
	}
	FinishTime = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = StartTime - FinishTime;
	//std::cout << diff.count() << std::endl;
	//std::cout << "Finished rendering from " << xmin << " to " << xmax << std::endl;
}

bool rayTriangleIntersect_func(
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

int			main(int ac, char* av[])
{
	t_sdl sdl;
	Uint8 run;
	t_ray ray;
	const auto aspect_ratio = (double)RENDERW / RENDERH;

	//const int image_width = RENDERW;
	//const int image_height = static_cast<int>(image_width / aspect_ratio);

	//// Camera

	//file.open("CurrOutput.txt");
	log_file.open("logs.txt");

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = Vec3d(0, 0, 0);
	auto horizontal = Vec3d(viewport_width, 0, 0);
	auto vertical = Vec3d(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3d(0, 0, focal_length);

	//image = IMG_Load("C:\\Users\\ Ryan\\Desktop\\Pics\\GRAF_1560262949284.png");
	run = 1;
	ray.origin = origin;
	Vec3d top_left_corner = Vec3d(-1.0, -1.0, cameraZ);
	Vec3d x_axis = Vec3d(1.0, 0.0, 0.0);
	Vec3d y_axis = Vec3d(0.0, 1.0, 0.0);
	Vec3d color = {0, 0, 0};


	ShapeContainer scene;


	auto left = make_shared<Lambertian>(Vec3d(0.8, 0.8, 0.8));
	auto right = make_shared<Lambertian>(Vec3d(0.8, 0.5, 0.2));
	auto center = make_shared<Lambertian>(Vec3d(0, 1, 0));
	auto cube = make_shared<Lambertian>(Vec3d(0, 0, 1));
	auto ground = make_shared<Lambertian>(Vec3d(1, 1, 1));
	auto def_metal = make_shared<Metal>(Vec3d(0.5, 0.5, 0.5));
	std::cout << cube << std::endl;
	std::vector<Triangle> tris;
	tris.push_back(Triangle(Vec3d(0, 0, 0), Vec3d(0, 1, 0), Vec3d(1, 1, 0)));
	tris.push_back(Triangle(Vec3d(0, 0, 0), Vec3d(1, 0, 0), Vec3d(1, 1, 0)));
	tris.push_back(Triangle(Vec3d(0, 0, 0), Vec3d(0, 1, 0), Vec3d(1, 1, 0)));
	tris.push_back(Triangle(Vec3d(1, 1, 0), Vec3d(1, 0, 0), Vec3d(0, 0, 0)));
	tris.push_back(Triangle(Vec3d(1, 0, 1), Vec3d(1, 1, 1), Vec3d(0, 1, 1)));
	tris.push_back(Triangle(Vec3d(1, 0, 0), Vec3d(0, 1, 0), Vec3d(0, 0, 0)));
	tris.push_back(Triangle(Vec3d(0, 0, 0), Vec3d(0, 2, 0), Vec3d(2, 0, 0)));
	scene.add(make_shared<Sphere>(Vec3d(0, -1000.5, -3), 1000.0, ground));
	scene.add(make_shared<Sphere>(Vec3d(0, 0, -3), 0.5, center));
	scene.add(make_shared<Sphere>(Vec3d(-1, 0, -3), 0.5, def_metal));
	scene.add(make_shared<Sphere>(Vec3d(1, 0, -3), 0.5, def_metal));
	scene.add(make_shared<Sphere>(Vec3d(-3, 1.5, -3), 2, def_metal));
	scene.add(make_shared<Model>(Vec3d(0, 0, -2), "chlendick.mod", cube));
	//scene.add(make_shared<Model>(Vec3d(0, 0, -1), tris, cube));

	if (!init_sdl(&sdl))
		return (0);
	//for (auto i = 0; i < 10; i++)
	//	std::cout << dot(normalized_vector(Vec3d{ 1, 1, 0 }), Vec3d{ 0, 1, 0 });
	//std::vector<std::thread> threads;
	const int num_threads = 5;
	const int samples_per_pixel = 100;

	std::chrono::time_point<std::chrono::system_clock> StartTime;
	std::chrono::time_point<std::chrono::system_clock> FinishTime;
	std::vector<std::future<void>> futures_vec;
	while (run)
	{
		StartTime = std::chrono::system_clock::now();
		int	nx = RenderW;
		int ny = RenderH;
		image_width = RenderW;
		int image_height = static_cast<int>(image_width / aspect_ratio);
		//scene.objects[1]->pos = default_sphere;
		//for (int i = 0; i < num_threads; i++)
		//{
		//	threads.push_back(std::thread(render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
		//}

		//for (auto& th : threads)
		//{
		//	th.join();
		//}
		//threads.clear();
		for (int i = 0; i < num_threads; i++) {
			futures_vec.push_back(std::async(std::launch::async, render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
		}
		SDL_Rect src = { 0 , 0, nx, ny };
		SDL_Rect dst = { 0, 0, W, H };
		render_surface(sdl.ren, sdl.screen_surf, &src, NULL);
//		for (int y = 0; y <= ny; y++)
//		{
//			for (int x = 0; x < nx; x++)
//			{
//#ifdef ANTIALIASING
//				color = { 0, 0, 0 };
//				for (int i = 0; i < samples_per_pixel; i++)
//				{
//					//std::cout << i << std::endl;
//					double u = (x + random_double()) / ((double)image_width - 1);
//					double v = (y + random_double()) / ((double)image_height- 1);
//					ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
//					color += ray_color(ray, scene, 50);
//
//				}
//				//log(color);
//				put_antialiased_pixel(sdl.screen_surf, x, ny - y, color, samples_per_pixel);
//#else
//				double u = x / ((double)image_width - 1);
//				double v = y / ((double)image_height - 1);
//				ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
//				color = ray_color(ray, scene, 50);
//				put_pixel(sdl.screen_surf, x, ny - y, SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255), (Uint8)(color.z * 255)});
//#endif // ANTIALIASING
//			}
//		}
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		//lightSource.x = (mx - W / 2.0) * 1000;
		//lightSource.y = (H / 2.0 - my) * 1000;
		Vec3d mousepos = top_left_corner + (((double)my) / ny) * y_axis + (((double)mx) / nx) * x_axis;
		//prinVec3d(mousepos);


		run = event_manager(&sdl, scene);
		FinishTime = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = FinishTime - StartTime;
		//std::cout << "MAIN CYCLE " << diff.count() << std::endl;
	}
	//Vec3d orig = Vec3d(0, 0, 0);
	//Vec3d dir = Vec3d(0, 0, 1);
	//Vec3d vec1 = Vec3d(-3, -3, 1);
	//Vec3d vec2 = Vec3d(-3, 3, 1);
	//Vec3d vec3 = Vec3d(5, 3, 1);
	//Vec3d AB = (vec2 - vec1);
	//Vec3d BC = (vec3 - vec2);
	//Vec3d CA = (vec1 - vec3);
	//double t;
	//std::cout << rayTriangleIntersect(orig, dir, vec3, vec2, vec1, t) << std::endl;
	//std::cout << intersect_plane_func(AB.cross(BC).normalized(), vec1,  Vec3d(0, 0, 0) , Vec3d(0, 0, 1), t);

}

//int			main(int ac, char *av[])
//{
//	t_sdl sdl;
//	Uint8 run;
//	t_ray ray;
//
//	run = 1;
//	int nx = 200;
//	int ny = 100;
//	image = IMG_Load("C:\\Users\\ Ryan\\Desktop\\Pics\\GRAF_1560262949284.png");
//	Vec3d lower_left_corner = {-2.0, -1.0, -1.0};
//	Vec3d horizontal = {4.0, 0.0, 0.0};
//	Vec3d vertical = {0.0, 2.0, 0.0};
//	Vec3d origin = {0.0, 0.0, 0.0};
//	if (!init_sdl(&sdl))
//		return (0);
//	while (run)
//	{
//		for (int j = ny - 1; j >= 0; j--)
//		{
//			for (int i = 0; i < nx; i++)
//			{
//				double u = (double)i / (double)nx;
//				double v = 1.0 - (double)j / (double)ny;
//				ray =  (t_ray){origin, lower_left_corner + u * horizontal + v * vertical};
//				//prinVec3d(ray.direction);
//				SDL_Color color = ray_color(ray);
//				put_pixel(sdl.screen_surf, i, j, color);
//			}
//		}
//		int mx, my;
//		SDL_GetMouseState(&mx, &my);
//		double u = (double)mx / (double)nx;
//		double v = (double)my / (double)ny;
//		Vec3d mousepos = lower_left_corner + u * horizontal + v * vertical;
//		prinVec3d(mousepos);
//		render_surface(sdl.ren, sdl.screen_surf, NULL, NULL);
//		run = event_manager(&sdl);
//	}
//	return (0);
//}
