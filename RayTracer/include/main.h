#pragma once
#ifndef MAIN_HPP
#define MAIN_HPP

#define W 1600
#define H 900
#define RENDERW 400
#define RENDERH 225
#define ANTIALIASINGs
#define THREADS
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <valarray>
#include <iostream>
#include <vector>
#include "Vec3d.h"
#include <limits>
#include <future>


constexpr int samples_per_pixel = 50;
constexpr int num_threads = RENDERW;

const double infinity = std::numeric_limits<double>::infinity();

typedef std::valarray<double> t_vector;

double clamp(double n, double min, double max)
{
	if (n < min)
		return min;
	if (n > max)
		return max;
	return n;
}

double random_double()
{
	return ((double)rand() / RAND_MAX);
}

double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

Vec3d random_vec()
{
	return (Vec3d(random_double(), random_double(), random_double()));
}

Vec3d random_vec(double min, double max)
{
	return (Vec3d(random_double(min, max), random_double(min, max), random_double(min, max)));
}

Vec3d random_unit_vector()
{
	auto a = random_double(0, 2 * M_PI);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);
	return Vec3d(r * cos(a), r * sin(a), z);
}

Vec3d random_in_unit_sphere()
{
	while (true)
	{
		Vec3d vec = random_vec(-1, 1);
		if (vec.length_squared() >= 1) continue;
		return vec;
	}
}

struct		t_sdl
{
	SDL_Surface* screen_surf;
	SDL_Window* win;
	SDL_Renderer* ren;

};


#endif // MAIN_H
