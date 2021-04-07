#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <chrono>
#include "../include/main.h"
#include "../include/Vec3d.h"
#include "../include/Ray.h"
#include "../include/Sphere.h"
#include "../include/ShapeContainer.h"
#include "../include/Materials.h"
#include "../include/Plane.h"
#include "../include/Model.h"
#include "../include/Timer.h"
#pragma warning

Vec3d WHITE = Vec3d(1, 1, 1);
Vec3d RED = Vec3d(1, 0, 0);
Vec3d GREEN = Vec3d(0, 1, 0);
Vec3d BLUE = Vec3d(0, 0, 1);
Vec3d BLACK = Vec3d(0, 0, 0);


double cornerZ = -1.0;
SDL_Surface* image;
Vec3d default_sphere = { 0, 0, -4 };
double sphereR = 0.5;
double cameraZ = -1.0;


std::vector<Vec3d> lightSources;
Vec3d lightSource = Vec3d(0, 17, 0);
Vec3d lightIntensity = { 1 ,1 , 1 };
Vec3d lightColor = { 1, 1, 1};
int n = 20;

std::vector<Vec3d> shapes;

int RenderW = RENDERW;
int RenderH = RENDERH;

int image_width = RenderW;

std::ofstream log_file;

Vec3d velocity{ 0, 0, -2 };

bool cameraTurn = false;
bool frameRendered = false;

Vec3d lookfrom{ 0, 5, 0.000001 };
Vec3d lookat{ 0, 0, 0 };
Vec3d vup{ 0, 1, 0 };

double Ks = 1;
double Kd = 1;

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



Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
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
	IMG_Init(IMG_INIT_PNG & IMG_INIT_PNG);
	sdl->win = SDL_CreateWindow("Win", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS);
	if (!sdl->win) {
		printf("%s\n", SDL_GetError());
		return (0);
	}
	sdl->ren = SDL_CreateRenderer(sdl->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	sdl->screen_surf = SDL_CreateRGBSurface(0, W, H, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	return (1);
}

bool mouseDown = false;
bool ctrlDown = false;

int			event_manager(t_sdl* sdl, ShapeContainer& scene)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			return (0);
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z)
		{
			if (ctrlDown)
				lightSource.z += 200;
			else
				lightSource.z += 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
		{
			if (ctrlDown)
				lightSource.z -= 200;
			else
				lightSource.z -= 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT)
		{
			if (ctrlDown)
				lightSource.x += 200;
			else
				lightSource.x += 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT)
		{
			if (ctrlDown)
				lightSource.x -= 200;
			else
				lightSource.x -= 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
		{
			if (ctrlDown)
				lightSource.y -= 200;
			else
				lightSource.y -= 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP)
		{
			if (ctrlDown)
				lightSource.y += 200;
			else
				lightSource.y += 2;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LCTRL)
		{
			std::cout << "Ctrl down\n";
			ctrlDown = true;
		}
		else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_LCTRL)
		{
			std::cout << "Ctrl up\n";
			ctrlDown = false;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == '-')
		{
			lightIntensity -= 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_EQUALS)
		{
			lightIntensity += 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'n' && ctrlDown == false)
		{
			n += 10;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'n' && ctrlDown)
		{
			n -= 10;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'r' && ctrlDown == false)
		{
			lightColor += Vec3d{ 0.1, 0, 0 };
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'r' && ctrlDown)
		{
			lightColor -= Vec3d{ 0.1, 0, 0 };
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'g' && ctrlDown == false)
		{
			lightColor += Vec3d{ 0, 0.1, 0 };
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'g' && ctrlDown)
		{
			lightColor -= Vec3d{ 0, 0.1, 0 };
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'b' && ctrlDown == false)
		{
		lightColor += Vec3d{ 0, 0, 0.1 };
		cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'b' && ctrlDown)
		{
		lightColor -= Vec3d{ 0, 0, 0.1 };
		cameraTurn = true;
		}
		else if (event.type == SDL_MOUSEWHEEL) {
			lookfrom = lookfrom.normalized() * ((lookfrom - lookat).length() - event.wheel.y);
			cameraTurn = true;
			//printf("%d\n", event.wheel.y);
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			std::cout << "MOuseBUtton pressed\n";
			mouseDown = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 's' && ctrlDown == false)
		{
			Ks += 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 's' && ctrlDown)
		{
			Ks -= 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'd' && ctrlDown == false)
		{
			Kd += 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'd' && ctrlDown)
		{
			Kd -= 0.1;
			cameraTurn = true;
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
			mouseDown = false;
		scene.objects[0]->pos = lightSource;
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

inline Vec3d reflectVec(const Vec3d& vec, const Vec3d& surface_normal)
{
	return (vec - 2 * vec.dot(surface_normal) * surface_normal);
}

Vec3d phongShading(t_ray ray, hit_record rec, const ShapeContainer& scene, Vec3d color)
{
	Vec3d res = {1, 1, 1};
	for (int i = 0; i < lightSources.size(); i++)
	{
		t_ray lightRay = { rec.hit_pos, (lightSources[i] - rec.hit_pos).normalized() };
		Vec3d diffuse = { 0, 0, 0 }, specular = { 0, 0, 0 };
		Vec3d lightDir = (lightSources[i] - rec.hit_pos).normalized();
		hit_record tempRec;



		bool vis = !(scene.hit(lightRay, 0.000001, infinity, tempRec) && (tempRec.hit_pos - lightSources[i]).length() >= 1);
		//vis = true;
		// compute the diffuse component
		diffuse += vis * color * std::max(0.0, rec.normal.normalized().dot(lightDir * -1));// * lightColor;
		// compute the specular component
		// what would be the ideal reflection direction for this light ray
		Vec3d R = reflectVec(lightDir, rec.normal.normalized());
		specular += vis * std::pow(std::max(0.0, R.dot(ray.direction.normalized())), n);
		//return attenuation * clamp(shadow, 0, 1);
		res *= diffuse * Kd + specular * Ks;
		res *= lightColor;

	}
	res.x = clamp(res.x, color.x / 5, 1);
	res.y = clamp(res.y, color.y / 5, 1);
	res.z = clamp(res.z, color.z / 5, 1);
	return res;
}

Vec3d	ray_color(const t_ray& ray, const ShapeContainer& scene, int depth)
{
	double t;
	//int *pix = (int*)image->pixels;
	hit_record rec;

	if (depth <= 0 || cameraTurn) {
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




	if (scene.hit(ray, 0.000000000001, infinity, rec))
	{
		t_ray scattered;
		Vec3d attenuation;
		//std::cout << rec.mat_ptr << std::endl;

		if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
		{

			//return rec.hit_color;
			//std::cout << attenuation << std::endl;
			hit_record tempRec;

			double shadow = 1;


			//if (l > 1)
			//{
			//	attenuation.x = clamp(attenuation.x + (0.5 - attenuation.x) * (clamp(l, 0, lmax) / lmax), 0, 1);
			//	attenuation.y = clamp(attenuation.y + (0.5 - attenuation.y) * (clamp(l, 0, lmax) / lmax), 0, 1);
			//	attenuation.z = clamp(attenuation.z + (0.5 - attenuation.z) * (clamp(l, 0, lmax) / lmax), 0, 1);
			//}
			//Vec3d diffuse = { 0, 0, 0 }, specular = { 0, 0, 0 };
			//Vec3d lightDir = (lightSource - rec.hit_pos).normalized();

			//int n = 20;
			//double Ks = 1;
			//double Kd = 1;
			//bool vis = !(scene.hit(lightRay, 0.001, infinity, tempRec) && (tempRec.hit_pos - lightSource).length() >= 1);

			//// compute the diffuse component
			//diffuse += vis * attenuation * 1 * std::max(0.0, rec.normal.normalized().dot(lightDir * -1));
			//// compute the specular component
			//// what would be the ideal reflection direction for this light ray
			//Vec3d R = reflectVec(lightDir, rec.normal.normalized());
			//specular += vis * 1 * std::pow(std::max(0.0, R.dot(ray.direction.normalized())), n);
			////return attenuation * clamp(shadow, 0, 1);
			//Vec3d res = diffuse * Kd + specular * Ks;
			//Vec3d lightColor = { 0, 0, 1 };
			//res.x = clamp(res.x, 0, 1);
			//res.y = clamp(res.y, 0, 1);
			//res.z = clamp(res.z, 0, 1);
			//std::cout << res << std::endl;
			//return attenuation * clamp(shadow, 0, 1);
			//attenuation = { 1, 0, 0 };
			//attenuation = phongShading(ray, rec, scene, attenuation);
			return (attenuation * ray_color(scattered, scene, depth - 1));
			//return (attenuation * ray_color(scattered, scene, depth - 1) * shadow);
			//return attenuation;
		}
		else
		{
			double l = (lookfrom - rec.hit_pos).length();
			double fog = 1;
			const double lmax = 50;

			Vec3d shadedPixel = phongShading(ray, rec, scene, attenuation);
			if (l > 1)
			{
				shadedPixel.x = clamp(shadedPixel.x + (0.5 - shadedPixel.x) * (clamp(l, 0, lmax) / lmax), 0, 1);
				shadedPixel.y = clamp(shadedPixel.y + (0.5 - shadedPixel.y) * (clamp(l, 0, lmax) / lmax), 0, 1);
				shadedPixel.z = clamp(shadedPixel.z + (0.5 - shadedPixel.z) * (clamp(l, 0, lmax) / lmax), 0, 1);
			}
			return shadedPixel;
		}
		return (Vec3d(0, 0, 0));
	}
	Vec3d normalized_dir = ray.direction.normalized();
	t = 0.5 * (normalized_dir.y + 1.0);
	Vec3d color = (1.0 - t) * Vec3d(1.0, 1.0, 1.0) + t * Vec3d(0.5, 0.7, 1.0);
	//std::cout << "Color in func: " << color << std::endl;
	color = { 0.5, 0.5, 0.5 };
	return (color);
}


void rotate_vecX(Vec3d& vec, double angle)
{
	angle *= 0.0174533;
	vec = { vec.x, vec.y * cos(angle) - vec.z * sin(angle), vec.y * sin(angle) + vec.z * cos(angle) };
}

void rotate_vecY(Vec3d& vec, double angle)
{
	angle *= 0.0174533;
	vec = { vec.x * cos(angle) + vec.z * sin(angle), vec.y, -vec.x * sin(angle) + vec.z * cos(angle) };
}

void rotate_vecZ(Vec3d& vec, double angle)
{
	angle *= 0.0174533;
	vec = { vec.x * cos(angle) - vec.y * sin(angle), vec.x * sin(angle) + vec.y * cos(angle),  vec.z };
}

#include <mutex>

static std::mutex sdlSurfaceMutex;


void render_column(int xmin, int xmax, t_sdl& sdl, ShapeContainer& scene)
{
	std::chrono::time_point<std::chrono::system_clock> StartTime;
	std::chrono::time_point<std::chrono::system_clock> FinishTime;
	//std::cout << "Started rendering from " << xmin << " to " << xmax << std::endl;
	const auto aspect_ratio = (double)RenderW / RenderH;
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto w = (lookfrom - lookat).normalized();
	auto u = vup.cross(w).normalized();
	auto v = w.cross(u);

	auto origin = lookfrom;
	auto horizontal = viewport_width * u;
	auto vertical = viewport_height * v;
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;


	Vec3d color = { 0, 0, 0 };
	t_ray ray;
	ray.origin = origin;

	int	nx = RenderW;
	int ny = RenderH;
	image_width = RenderW;
	int image_height = static_cast<int>(image_width / aspect_ratio);
	StartTime = std::chrono::system_clock::now();


	for (int y = 0; y <= ny; y++)
	{
		for (int x = xmin; x < xmax; x++)
		{
			if (cameraTurn)
				return;
#ifdef ANTIALIASING
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
			//std::cout << "Uv took " << uv.microsecPassed() << std::endl;
			color = ray_color(ray, scene, 50);
			std::lock_guard<std::mutex> lock(sdlSurfaceMutex);
			put_pixel(sdl.screen_surf, x, ny - y, SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255), (Uint8)(color.z * 255) });
#endif // ANTIALIASING
		}
	}
	//std::cout << diff.count() << std::endl;
	//std::cout << "Finished rendering from " << xmin << " to " << xmax << std::endl;
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

	image = IMG_Load("ComeHere.jpg");


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
	Vec3d color = { 0, 0, 0 };


	ShapeContainer scene;


	auto left_mat = make_shared<Lambertian>(Vec3d(0.8, 0.8, 0.8));
	auto right_mat = make_shared<Lambertian>(Vec3d(0.8, 0.5, 0.2));
	auto center_mat = make_shared<Lambertian>(Vec3d(0, 1, 0));
	auto cube_mat = make_shared<Lambertian>(Vec3d(0, 0, 1));
	auto ground_mat = make_shared<Metal>(Vec3d(1, 1, 1));
	auto def_metal = make_shared<Metal>(Vec3d(0.8, 0.8, 0.8), 0);
	auto model_mat = make_shared<Metal>(Vec3d(0.5, 0.5, 0.8), 0);
	auto lambTexture = make_shared<LambertianTexture>(Vec3d(0, 0, 0));
	auto metalTexture = make_shared<MetalTexture>();
	auto sunMaterial = make_shared<Lambertian>(Vec3d(1, 1, 0));
	auto plainTexture = make_shared<PlainTexture>();
	std::vector<shared_ptr<Material>> matVec;
	matVec.push_back(left_mat);
	matVec.push_back(right_mat);
	matVec.push_back(center_mat);
	matVec.push_back(ground_mat);
	matVec.push_back(cube_mat);
	matVec.push_back(def_metal);

	lightSources.push_back({ 0, 17, 0 });
	lightSources.push_back({ -5, 17, 0 });

	std::vector<Triangle> tris;
	//tris.push_back(Triangle(Vec3d(0, 0, 0), Vec3d(0, 1, 0), Vec3d(1, 0, 0), { {0, 1}, {0 , 0}, {1, 0} }));
	//tris.push_back(Triangle(Vec3d(1, 1, 0), Vec3d(1, 0, 0), Vec3d(0, 0, 0), { {1, 0}, {1, 1}, {0, 1} }));
	//shared_ptr<Model> test = make_shared<Model>(Vec3d(0, 0, 0), tris, center_mat);
	//test->texture = IMG_Load("Red.png");
	//for (auto& tri : test->triangles)
	//	std::cout << tri.p1 << ", " << tri.p2 << ", " << tri.p3 << std::endl;
	//test->rotateX(180);
	//for (auto& tri : test->triangles)
	//	std::cout << tri.p1 << ", " << tri.p2 << ", " << tri.p3 << std::endl;
	//scene.add(make_shared<Sphere>(Vec3d(0, -1005, -3), 1000.0, def_metal));
	//scene.add(make_shared<Sphere>(Vec3d(0, 2.5, 2), 2, center_mat));
	scene.add((make_shared<Sphere>(lightSource, 0.5, sunMaterial)));
	scene.add(make_shared<Sphere>(Vec3d(-10, 2.5, 2), 2, def_metal));
	//scene.add(make_shared<Sphere>(Vec3d(10, 2.5, 2), 2, def_metal));
	//scene.add(make_shared<Sphere>(Vec3d(-3, 1.5, -3), 2, def_metal));
	//shared_ptr<Model> teapot = make_shared<Model>(Vec3d(0, 0, -3), "teapot.obj", center_mat);
	//shared_ptr<Model> cube = make_shared<Model>(Vec3d(0, -1, 0), "TexturedCube.obj", plainTexture);
	//cube->resize(1);
	//scene.add(cube);
	//shared_ptr<Model> ground_cube = make_shared<Model>(Vec3d(0, -35, -10), "TexturedCube.obj", plainTexture);
	//ground_cube->texture = IMG_Load("Textures/ComeHere.jpg");
	//ground_cube->resize(30);
	//scene.add(ground_cube);
	//shared_ptr<Model> sword = make_shared<Model>(Vec3d(0, -2, -3), "chlendick.obj", metalTexture);
	//shared_ptr<Model> rotated_cube = make_shared<Model>(Vec3d(4, 1, 6), "RotatedTexturedCube.obj", plainTexture);
	//shared_ptr<Model> hyper_rotated_cube = make_shared<Model>(Vec3d(-4, 1, 6), "HyperRotatedTexturedCube.obj", plainTexture);
	//shared_ptr<Model> deer = make_shared<Model>(Vec3d(0, 5, -5), "DeerTriangulated.obj", plainTexture);
	//deer->resize(0.02);
	//scene.add(deer);
	shared_ptr<Model> sphere = make_shared<Model>(Vec3d(0, -1, 0), "Sphere.obj", plainTexture);
	shared_ptr<Model> table = make_shared<Model>(Vec3d(0, 0, 5), "TableTriangulated.obj", plainTexture);
	//table->texture = IMG_Load("Textures/T_Table_Albedo.png");
	//table->resize(2);
	//scene.add(table);
	//shared_ptr<Model> cube = make_shared<Model>(Vec3d(0, -1, 0), "TexturedCube.obj", plainTexture);
	//scene.add(cube);

	//scene.add(sphere);
	shared_ptr<Model> catWalk = make_shared<Model>(Vec3d(0, -1, 0), "CatWalk.obj", plainTexture);
	//scene.add(catWalk);
	//shared_ptr<Model> rectBox = make_shared<Model>(Vec3d(0, -1, 0), "RectBox.obj", plainTexture);
	//scene.add(rectBox);
	shared_ptr<Model> bullet = make_shared<Model>(Vec3d(0, 1, 0), "Bullet.obj", plainTexture);
	bullet->texture = IMG_Load("../RayTracer/Textures/Bullet_BaseColor.png");
	scene.add(bullet);
	


	std::shared_ptr<Plane> plane = make_shared<Plane>(Vec3d(0, -3, 0), Vec3d(0, -2, 0), metalTexture);
	scene.add(plane);

	//scene.add(sword);
	//scene.add(cube);
	//shared_ptr<Model> roman = make_shared<Model>(Vec3d(0, 0, -3), "roman.obj", center_mat);
	//shared_ptr<Model> bevel = make_shared<Model>(Vec3d(0, 0, -3), "bevel.obj", center_mat);

	//rotate_vecX(lookfrom, -30);
	//rotate_vecZ(lookfrom, -50);
	//rotate_vecY(lookfrom, 30);

	//scene.add(test);
	//scene.add(roman);
	//scene.add(rotated_cube);
	//scene.add(hyper_rotated_cube);
	//scene.add(make_shared<Sphere>(Vec3d(0, 0, -2), 0.3, center));
	//scene.add(make_shared<Sphere>(mod->center + mod->pos + Vec3d(mod->encircling_shpere_radius + 1, 0, 0), mod->encircling_shpere_radius, center));
	//scene.add(make_shared<Model>(Vec3d(0, 0, -1), tris, cube));

	if (!init_sdl(&sdl))
		return (0);
	//for (auto i = 0; i < 10; i++)
	//	std::cout << dot(normalized_vector(Vec3d{ 1, 1, 0 }), Vec3d{ 0, 1, 0 });
	std::vector<std::thread> threads;
	//const int num_threads = std::thread::hardware_concurrency();

	std::chrono::time_point<std::chrono::system_clock> StartTime;
	std::chrono::time_point<std::chrono::system_clock> FinishTime;
	std::vector<std::future<void>> futures_vec;
	int currWidth = 80;
	int currHeight = 45;
	int prevMx, prevMy;
	SDL_GetMouseState(&prevMx, &prevMy);
#ifdef THREADS

	//Timer renderInThreads;
	//for (int i = 0; i < num_threads; i++) {
	//	futures_vec.push_back(std::async(std::launch::async, render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
	//}
	//
	//for (int i = 0; i < num_threads; i++) {
	//	futures_vec[i].wait();
	//}
	//std::cout << "Render in threads took " << renderInThreads.diffMS() << " ms\n";
	//Timer renderInThreads;
	//for (int i = 0; i < num_threads; i++)
	//{
	//	threads.push_back(std::thread(render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
	//}

	//for (auto& th : threads)
	//{
	//	th.join();
	//}
	//threads.clear();
	//std::cout << "Render in threads took " << renderInThreads.diffMS() << " ms\n";
#endif //THREADS
	while (run)
	{
		//Timer fullRender;
		int	nx;
		int ny;
		//image_width = RenderW;
		//int image_height = static_cast<int>(image_width / aspect_ratio);
		Timer highResFrame;
		while (currWidth <= 1600/*RENDERW*/)
		{
			//int currWidth = 400;
			//int currHeight = 225;
			frameRendered = false;
			cameraTurn = false;
			RenderW = currWidth;
			RenderH = currHeight;
			nx = RenderW;
			ny = RenderH;
			
			for (int i = 0; i < num_threads; i++)
			{
				futures_vec.push_back(std::async(std::launch::async, render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
			}

			//SDL_Rect src = { 0 , 0, nx, ny };
			//SDL_Rect dst = { 0, 0, W, H };
			//render_surface(sdl.ren, sdl.screen_surf, &src, NULL);

			//lightSource.x = (mx - W / 2.0) * 1000;
			//lightSource.y = (H / 2.0 - my) * 1000;
			//prinVec3d(mousepos);

			int prevMx, prevMy;

			SDL_GetMouseState(&prevMx, &prevMy);
			int i = 0;
			while (!cameraTurn && !frameRendered && run)
			{
				//std::cout << ++i << std::endl;;
				run = event_manager(&sdl, scene);
				//if (mouseDown)
				//	std::cout << "asdasd\n";
				int mx, my;
				SDL_GetMouseState(&mx, &my);
				if (mouseDown && (prevMx - mx != 0 || prevMy - my != 0))
				{
					rotate_vecX(lookfrom, (my - prevMy) / 3.0);
					rotate_vecY(lookfrom, (prevMx - mx) / 3.0);

					cameraTurn = true;
					//std::cout << "Camera turned\n";
				}
				prevMx = mx;
				prevMy = my;
				frameRendered = true;
				for (int i = 0; i < num_threads; i++) {
					if (!futures_vec[i]._Is_ready())
						frameRendered = false;
				}
				
			}
			if (cameraTurn)
			{
				currWidth = 50;
				currHeight = 29;
				cameraTurn = false;
				futures_vec.clear();
				continue;
			}
			//if (!run)
			//{
			//for (int i = 0; i < num_threads; i++) {
			//	futures_vec[i].wait();
			//}
			futures_vec.clear();
			SDL_Rect src = { 0 , 0, nx, ny };
			SDL_Rect dst = { 0, 0, W, H };
			render_surface(sdl.ren, sdl.screen_surf, &src, NULL);

			//}

			//using namespace std::chrono_literals;
			//std::this_thread::sleep_for(2s);
			currWidth *= 2;
			currHeight *= 2;
			if (currWidth > 1600)
				std::cout << "Rendering in " << currWidth / 2 << "x" << currHeight / 2 << " took " << highResFrame.diffMS() << " ms" << std::endl;
		}
		
		run = event_manager(&sdl, scene);
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		if (mouseDown && (prevMx - mx != 0 || prevMy - my != 0))
		{
			rotate_vecY(lookfrom, (prevMx - mx) / 3.0);
			rotate_vecX(lookfrom, (my - prevMy) / 3.0);

			//std::cout << mx - prevMx << " " << my - prevMy << std::endl;
			cameraTurn = true;
			//std::cout << "Camera turned\n";
		}
		prevMx = mx;
		prevMy = my;
		if (cameraTurn)
		{
			currWidth = 50;
			currHeight = 29;
			cameraTurn = false;
			continue;
		}
		//if (!run)
		//{
		//	futures_vec.clear();

		//}
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
		//cube->rotateY(10);
		//cube->rotateX(15);
		//cube->rotateZ(5);
		//rotated_cube->rotateY(10);
		//rotated_cube->rotateX(15);
		//rotated_cube->rotateZ(5);
		//hyper_rotated_cube->rotateY(10);
		//hyper_rotated_cube->rotateX(15);
		//hyper_rotated_cube->rotateZ(5);
		//rotate_vecY(lookfrom, 5);
		//lookfrom += Vec3d{ 0, 0, 0.5 };

		//rotate_vecY(lightSource, -10);
		//scene.objects[2]->pos = lightSource;

#ifdef THREADS
		//futures_vec.clear();
		//for (int i = 0; i < num_threads; i++) {
		//	futures_vec.push_back(std::async(std::launch::async, render_column, i * RenderW / num_threads, (i + 1) * RenderW / num_threads, std::ref(sdl), std::ref(scene)));
		//}
		//for (auto& future : futures_vec)
		//	future.wait();
#else
		long long  rayColorTime = 0;
		for (int y = 0; y <= ny; y++)
		{
			for (int x = 0; x < nx; x++)
			{
#ifdef ANTIALIASING
				color = { 0, 0, 0 };
				//Timer rayColor;
				rayColorTime = 0;
				for (int i = 0; i < samples_per_pixel; i++)
				{
					//std::cout << i << std::endl;
					double u = (x + random_double()) / ((double)image_width - 1);
					double v = (y + random_double()) / ((double)image_height - 1);
					ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
					Timer rayColor;
					color += ray_color(ray, scene, 50);
					rayColorTime = rayColor.diff();

				}

				//std::cout << "ray_color" << rayColor.diff() << std::endl;
				//log(color);
				put_antialiased_pixel(sdl.screen_surf, x, ny - y, color, samples_per_pixel);
#else
				double u = x / ((double)image_width - 1);
				double v = y / ((double)image_height - 1);
				ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
				Timer timer;
				color = ray_color(ray, scene, 50);
				//std::cout << "ray_Color took " << timer.diffMS() << " ms\n";
				put_pixel(sdl.screen_surf, x, ny - y, SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255), (Uint8)(color.z * 255) });
#endif // ANTIALIASING
			}
		}
		std::cout << "ray_color: " << rayColorTime << std::endl;
#endif // THREADS
		//SDL_Rect src = { 0 , 0, nx, ny };
		//SDL_Rect dst = { 0, 0, W, H };
		//render_surface(sdl.ren, sdl.screen_surf, &src, NULL);
		//int mx, my;
		//SDL_GetMouseState(&mx, &my);
		////lightSource.x = (mx - W / 2.0) * 1000;
		////lightSource.y = (H / 2.0 - my) * 1000;
		//Vec3d mousepos = top_left_corner + (((double)my) / ny) * y_axis + (((double)mx) / nx) * x_axis;
		////prinVec3d(mousepos);


		//run = event_manager(&sdl, scene);
		//if (!run)
		//{
		//	futures_vec.clear();

		//}
		//std::cout << "Full Render: " << fullRender.diff() << std::endl;
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
