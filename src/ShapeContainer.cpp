#include "../include/ShapeContainer.h"
#include "../include/Timer.h"

bool ShapeContainer::hit(const t_ray& ray, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;


    Timer timer;
    for (const auto& object : objects) {
        if (object->hit(ray, t_min, closest_so_far, temp_rec)) {;
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
            //std::cout << rec.mat_ptr << std::endl;
        }
    }
    //std::cout << "Scene.hit took " << timer.diffMS() << " ms\n";
    //if (pixels == 1)
    //    std::cout << ray.direction << hit_anything << std::endl;;
    //if (hit_anything)
    //    std::cout << pixels << std::endl;

    return hit_anything;
}

void ShapeContainer::clear()
{
    objects.clear();
}
void ShapeContainer::add(shared_ptr<Shape> object)
{
    objects.push_back(object);
}