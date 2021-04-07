#include "E:\42\RT\main.h"
//#include "E:\Мое xdd\RayTracer\RayTracer\PosContainer.h"
#include <SDL.h>
#include <fstream>
#include <string>

std::ofstream file;



double cornerZ = -1.0;
SDL_Surface* image;
double sphereX = 0.0;
double sphereY = 0.0;
double sphereZ = -1.0;
double sphereR = 0.5;
double cameraZ = -1.0;
SDL_Color RED = { 255, 0, 0 };

t_vector lightSource = { 2, 0, -1 };

void		put_pixel(SDL_Surface* surf, int x, int y, SDL_Color color)
{
	int* pix;

	pix = (int*)surf->pixels;
	pix[x + surf->w * y] = SDL_MapRGB(surf->format, color.r, color.g, color.b);
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

int			event_manager(t_sdl* sdl)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			return (0);
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z)
			sphereZ += 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
			sphereZ -= 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT)
			sphereX += 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT)
			sphereX -= 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
			sphereY += 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP)
			sphereY -= 0.05f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
			sphereR += 0.5f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t)
			sphereR -= 0.5f;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_l)
			lightSource[2] -= 100.0;
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k)
			lightSource[2] += 100.0;
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

double		distance3d(t_vector vec1, t_vector vec2)
{
	double dx = vec2[0] - vec1[0];
	double dy = vec2[1] - vec1[1];
	double dz = vec2[2] - vec1[2];

	return (sqrtf(dx * dx + dy * dy + dz * dz));
}

void		print_vector(t_vector a)
{
	printf("{ %lf, %lf, %lf }\n", a[0], a[1], a[2]);
}

std::string		vec_to_string(t_vector a)
{
	return ("[" + std::to_string(a[0]) + ", " + std::to_string(a[1]) + ", " + std::to_string(a[2]) + "]");
}

double		vector_length(t_vector vec)
{
	return (sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
}

t_vector	normalized_vector(t_vector vec)
{
	return (vec / vector_length(vec));
}

double		dot(t_vector vec1, t_vector vec2)
{
	return (vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]);
}

t_vector		define_vec(t_vector origin, t_vector direction, double length)
{
	return (origin + direction * length);
}

double			hit_plane(t_vector point, t_vector norm, t_ray ray)
{
	t_vector oc = ray.origin - point;
	double	del = dot(ray.direction, norm);
	if (del != 0.0)
		return (-dot(oc, norm) / del);
	else
		return (-1);
}

double			hit_sphere(t_vector center, double radius, t_ray ray)
{
	t_vector oc = ray.origin - center;
	double a = vector_length(ray.direction) * vector_length(ray.direction);
	double half_b = dot(oc, ray.direction);
	double c = vector_length(oc) * vector_length(oc) - radius * radius;
	double discriminant = half_b * half_b - a * c;
	file << "ray.direction:" << vec_to_string(ray.direction) << std::endl;
	file << "oc: " << vec_to_string(oc) << std::endl;
	file << "a: " << a << std::endl;
	file << "half_b: " << half_b << std::endl;
	file << "c: " << c << std::endl;
	file << "discriminant: " << discriminant << std::endl;
	if (discriminant < 0)
		return -1;
	else
		return ((-half_b - sqrt(discriminant)) / a);
}


SDL_Color	ray_color(t_ray ray)
{
	double t;
	//int *pix = (int*)image->pixels;
	Uint8 r, g, b;

	t_vector point_on_plane = t_vector{ 0.0, -1.0, 0.0 };
	t_vector plane_norm = { 0.0, 1.0, 0.0 };
	if ((t = hit_plane(point_on_plane, plane_norm, ray)) > 0)
	{
		t_vector	intersection_point = define_vec(ray.origin, ray.direction, t);
		if (dot(intersection_point - point_on_plane, plane_norm) == 0.0
			&& distance3d(intersection_point, { 0, 0, 1 }) < 1)
			return (SDL_Color{ 255, 0, 0 });
	}
	if ((t = hit_sphere({ sphereX, sphereY, sphereZ }, sphereR, ray)) > 0.0)
	{
		t_vector norm = normalized_vector(define_vec(ray.origin, ray.direction, t) - t_vector{ sphereX, sphereY, sphereZ });
		t_vector color = 0.5 * (norm + 1);
		double shadow = 1 + dot(norm, normalized_vector(lightSource));
		//SDL_GetRGB(pix[(int)(color[0] * image->w) + image->w * (int)(color[1] * image->h)], image->format, &r, &g, &b);
		//return (SDL_Color{(Uint8)(255 * color[0]), (Uint8)(255 * color[1]), (Uint8)(255 * color[2])});
		return (SDL_Color{ (Uint8)(255 * (shadow / 2)), 0, 0 });
		//return (SDL_Color{ (Uint8)(255 * (int)shadow), 0, 0 });
		//return (RED);
	}
	t_vector normalized_dir = normalized_vector(ray.direction);
	t = 0.5 * (normalized_dir[1] + 1.0);
	t_vector color = (1.0 - t) * t_vector{ 1.0, 1.0, 1.0 } + t * t_vector{ 0.5, 0.7, 1.0 };
	return (SDL_Color{ (Uint8)(color[0] * 255.999), (Uint8)(color[1] * 255.999), (Uint8)(color[2] * 255.999) });
}

int			main(int ac, char* av[])
{
	t_sdl sdl;
	Uint8 run;
	t_ray ray;
	const auto aspect_ratio = RENDERW / RENDERH;
	const int image_width = RENDERW;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	// Camera

	file.open("PrevOutput.txt");

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = t_vector{ 0, 0, 0 };
	auto horizontal = t_vector{ viewport_width, 0, 0 };
	auto vertical = t_vector{ 0, viewport_height, 0 };
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - t_vector{ 0, 0, focal_length };

	//image = IMG_Load("C:\\Users\\ Ryan\\Desktop\\Pics\\GRAF_1560262949284.png");
	run = 1;
	ray.origin = { 0.0, 0.0, 0.0 };
	t_vector top_left_corner = { -1.0, -1.0, cameraZ };
	t_vector x_axis = { 1.0, 0.0, 0.0 };
	t_vector y_axis = { 0.0, 1.0, 0.0 };
	int	nx = RENDERW;
	int ny = RENDERH;
	if (!init_sdl(&sdl))
		return (0);
	printf("%f\n", distance3d(ray.origin, y_axis));
	//for (auto i = 0; i < 10; i++)
	//	std::cout << dot(normalized_vector(t_vector{ 1, 1, 0 }), t_vector{ 0, 1, 0 });

		//while (run)
		//{
			for (int y = 0; y <= ny; y++)
			{
				for (int x = 0; x < nx; x++)
				{
					double u = double(x) / ((double)image_width - 1);
					double v = double(y) / ((double)image_height - 1);
					ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;
					//print_vector(ray.direction);
					SDL_Color color = ray_color(ray);
					put_pixel(sdl.screen_surf, x, ny - y, color);
				}
			}
			file.close();
			int mx, my;
			SDL_GetMouseState(&mx, &my);
			lightSource[0] = mx - W / 2.0;
			lightSource[1] = H / 2.0 - my;
			t_vector mousepos = top_left_corner + (((double)my) / ny) * y_axis + (((double)mx) / nx) * x_axis;
			//print_vector(mousepos);
			SDL_Rect src = { 0, 0, nx, ny };
			SDL_Rect dst = { 0, 0, W, H };
			render_surface(sdl.ren, sdl.screen_surf, &src, NULL);
			run = event_manager(&sdl);
			std::cout << dot(normalized_vector(lightSource), t_vector{ -1, 0, -1 }) << std::endl;
		//}
	return (0);
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
//	t_vector lower_left_corner = {-2.0, -1.0, -1.0};
//	t_vector horizontal = {4.0, 0.0, 0.0};
//	t_vector vertical = {0.0, 2.0, 0.0};
//	t_vector origin = {0.0, 0.0, 0.0};
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
//				//print_vector(ray.direction);
//				SDL_Color color = ray_color(ray);
//				put_pixel(sdl.screen_surf, i, j, color);
//			}
//		}
//		int mx, my;
//		SDL_GetMouseState(&mx, &my);
//		double u = (double)mx / (double)nx;
//		double v = (double)my / (double)ny;
//		t_vector mousepos = lower_left_corner + u * horizontal + v * vertical;
//		print_vector(mousepos);
//		render_surface(sdl.ren, sdl.screen_surf, NULL, NULL);
//		run = event_manager(&sdl);
//	}
//	return (0);
//}

