#include "sphere.h"

#include <cmath>

#include "pathtracer/bsdf.h"
#include "util/sphere_drawing.h"

namespace CGL {
namespace SceneObjects {

bool Sphere::test(const Ray &r, double &t1, double &t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  double r_mag = dot(r.d, r.d), og_dir = 2.0f * dot((r.o - this->o), r.d), ogmag_rad = dot(r.o - this->o, r.o - this->o) - this->r2;
  double diff = (og_dir * og_dir) - (4 * r_mag * ogmag_rad);
  if(diff <= 0) {
    return false;
  }
  else {
    double t_low = (-og_dir - sqrt(diff)) / 2 / r_mag, t_high = (-og_dir + sqrt(diff)) / 2 / r_mag;
    t1 = t_high;
    t2 = t_low;
    double mint = min(t1, t2);
    if(mint >= r.min_t && mint <= r.max_t) {
      r.max_t = mint;
      return true;
    }
    return false;
  }
  //should not reach this point
  return false;
}

bool Sphere::has_intersection(const Ray &r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  return this->test(r, t1, t2);
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {
  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  if(!test(r, t1, t2)) {
    return false;
  }
  else {
    Vector3D prenorm = (t2 * r.d + r.o) - this->o;
    prenorm.normalize();
    i->n = prenorm;
    i->t = t1;
    i->bsdf = get_bsdf();
    i->primitive = this;
    return true;
  }
}

void Sphere::draw(const Color &c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

} // namespace SceneObjects
} // namespace CGL
