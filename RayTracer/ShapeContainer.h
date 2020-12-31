#pragma once
#ifndef SHAPECONTAINER_H
#define SHAPECONTAINER_H

#include "Shape.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class ShapeContainer : public Shape {
public:
    ShapeContainer() {}
    ShapeContainer(shared_ptr<Shape> object) { add(object); }

    void clear();
    void add(shared_ptr<Shape> object);

    virtual bool hit(
        const t_ray& r, double tmin, double tmax, hit_record& rec) const override;

public:
    std::vector<shared_ptr<Shape>> objects;
};

#endif