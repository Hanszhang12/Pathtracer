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
  double r_mag = dot(r.d, r.d), og_dir = 2 * dot((r.o - this->o), r.d), ogmag_rad = dot(r.o - this->o, r.o - this->o) - this->r2;
  double diff = (og_dir * og_dir) - (4 * r_mag * ogmag_rad);
  if(diff <= 0) {
    return false;
  }
  else {
    double t_low = (-og_dir - sqrt(diff)) / 2 / r_mag, t_high = (-og_dir + sqrt(diff)) / 2 / r_mag;
    if(t_low >= r.min_t && t_high <= r.max_t) {
      t1 = t_low;
      t2 = t_high;
      if (t_low > 0) {
        r.max_t = t_low;
      }
      else {
        r.max_t = t_high;
      }
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
  if(!has_intersection(r)) {
    return false;
  }
  else {
    Vector3D prenorm = (r.max_t * r.d + r.o) - this->o;
    i->n = prenorm;
    prenorm.normalize();
    i->t = r.max_t;
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
