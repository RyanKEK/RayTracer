#pragma once
#ifndef VEC3D_H
#define VEC3D_H


#include <math.h>
#include <iostream>

class Vec3d
{
public:
	double x;
	double y;
	double z;


	Vec3d();
	Vec3d(double x, double y, double z);
	~Vec3d();

	double	length() const;
	double	length_squared() const;
	double	dot(Vec3d const & other) const;
	Vec3d	cross(Vec3d const& other) const;
	Vec3d	normalized() const;
	Vec3d & operator=(Vec3d const& right);
	bool operator==(Vec3d const& right);
	Vec3d operator+(Vec3d const & right) const;
	Vec3d operator-(Vec3d const & right) const;
	Vec3d operator*(Vec3d const & right) const;
	Vec3d operator/(Vec3d const & right) const;
	Vec3d & operator+=(Vec3d const& right);
	Vec3d & operator-=(Vec3d const& right);
	Vec3d & operator*=(Vec3d const& right);
	Vec3d & operator/=(Vec3d const& right);
	Vec3d& operator+=(double const& n);
	Vec3d& operator-=(double const& n);
	Vec3d& operator*=(double const& n);
	Vec3d& operator/=(double const& n);

};

inline Vec3d operator*(double n, const Vec3d& vec)
{
	return (Vec3d(vec.x * n, vec.y * n, vec.z * n));
}

inline Vec3d operator*(const Vec3d& vec, double n)
{
	return (n * vec);
}

inline Vec3d operator+(double n, const Vec3d& vec)
{
	return (Vec3d(vec.x + n, vec.y + n, vec.z + n));
}

inline Vec3d operator+(const Vec3d& vec, double n)
{
	return (n + vec);
}

inline Vec3d operator/(const Vec3d& vec, double n)
{
	return (Vec3d(vec.x / n, vec.y / n, vec.z / n));
}

inline std::ostream& operator<<(std::ostream& os, const Vec3d& vec)
{
	os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
	return os;
}

#endif // !VEC3D_H