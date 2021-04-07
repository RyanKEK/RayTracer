#include "../include/Vec3d.h"

Vec3d::Vec3d() : x(0.0), y(0.0), z(0.0)
{
}

Vec3d::Vec3d(double x, double y, double z) : x(x), y(y), z(z)
{
}

Vec3d::~Vec3d()
{
}

Vec3d Vec3d::cross(Vec3d const& other) const
{
	return (Vec3d(this->y * other.z - this->z * other.y, this->z * other.x - this->x * other.z, this->x * other.y - this->y * other.x));
}

double Vec3d::length() const
{
	return (sqrtf(this->x * this->x + this->y * this->y + this->z * this->z));
}

double Vec3d::length_squared() const
{
	return (this->x * this->x + this->y * this->y + this->z * this->z);
}

double Vec3d::dot(Vec3d const& other) const
{
	return (this->x * other.x + this->y * other.y + this->z * other.z);
}


Vec3d Vec3d::operator+(Vec3d const& right) const
{

	return (Vec3d(this->x + right.x, this->y + right.y, this->z + right.z));
}

Vec3d Vec3d::operator-(Vec3d const& right) const
{

	return (Vec3d(this->x - right.x, this->y - right.y, this->z - right.z));
}

Vec3d Vec3d::operator*(Vec3d const& right) const
{

	return (Vec3d(this->x * right.x, this->y * right.y, this->z * right.z));
}

Vec3d Vec3d::operator/(Vec3d const & right) const
{

	return (Vec3d(this->x / right.x, this->y / right.y, this->z / right.z));
}

Vec3d Vec3d::normalized() const
{
	Vec3d vec = *this;
	return (vec / this->length());
}

Vec3d& Vec3d::operator+=(Vec3d const& right)
{
	this->x += right.x;
	this->y += right.y;
	this->z += right.z;
	return (*this);
}

Vec3d& Vec3d::operator-=(Vec3d const& right)
{
	this->x -= right.x;
	this->y -= right.y;
	this->z -= right.z;
	return (*this);
}

Vec3d& Vec3d::operator*=(Vec3d const& right)
{
	this->x *= right.x;
	this->y *= right.y;
	this->z *= right.z;
	return (*this);
}

Vec3d& Vec3d::operator/=(Vec3d const& right)
{
	this->x /= right.x;
	this->y /= right.y;
	this->z /= right.z;
	return (*this);
}

Vec3d& Vec3d::operator=(Vec3d const& right)
{
	this->x = right.x;
	this->y = right.y;
	this->z = right.z;
	return (*this);
}


Vec3d& Vec3d::operator+=(double const& n)
{
	this->x += n;
	this->y += n;
	this->z += n;
	return (*this);
}

Vec3d& Vec3d::operator-=(double const& n)
{
	this->x -= n;
	this->y -= n;
	this->z -= n;
	return (*this);
}

Vec3d& Vec3d::operator*=(double const& n)
{
	this->x *= n;
	this->y *= n;
	this->z *= n;
	return (*this);
}

Vec3d& Vec3d::operator/=(double const& n)
{
	this->x /= n;
	this->y /= n;
	this->z /= n;
	return (*this);
}

bool Vec3d::operator==(Vec3d const& right)
{
	return (this->x == right.x && this->y == right.y && this->z == right.z);
}