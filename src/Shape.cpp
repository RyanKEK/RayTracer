#include "../include/Shape.h"

Shape::Shape()
{

}

Shape::Shape(Vec3d vec, std::shared_ptr<Material> mat) : pos(vec), mat_ptr(mat)
{
}

Shape::~Shape()
{
}