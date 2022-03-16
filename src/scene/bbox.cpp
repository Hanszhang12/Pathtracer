#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bounding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.

  if (t0 > t1) {
    return false;
  }

  double min_t_x = (min.x - r.o.x)/r.d.x;
  double min_t_y = (min.y - r.o.y)/r.d.y;
  double min_t_z = (min.z - r.o.z)/r.d.z;

  double max_t_x = (max.x - r.o.x)/r.d.x;
  double max_t_y = (max.y - r.o.y)/r.d.y;
  double max_t_z = (max.z - r.o.z)/r.d.z;

  double t_min_x, t_max_x;
  double t_min_y, t_max_y;
  double t_min_z, t_max_z;

  if (min_t_x >= max_t_x) {
    t_max_x = min_t_x;
    t_min_x = max_t_x;
  } else {
    t_max_x = max_t_x;
    t_min_x = min_t_x;
  }

  if (min_t_y >= max_t_y) {
    t_max_y = min_t_y;
    t_min_y = max_t_y;
  } else {
    t_max_y = max_t_y;
    t_min_y = min_t_y;
  }

  if (min_t_z >= max_t_z) {
    t_max_z = min_t_z;
    t_min_z = max_t_z;
  } else {
    t_max_z = max_t_z;
    t_min_z = min_t_z;
  }

  double new_t0 = std::max(std::max(t_min_x, t_min_y), t_min_z);
  double new_t1 = std::min(std::min(t_max_x, t_max_y), t_max_z);

  if (new_t0 > new_t1) {
    return false;
  }

  if ((t0 > new_t0 || t1 < new_t1) && (t0 > new_t0 && t1 > new_t1)){
    return false;
  } else {
    return true;
  }
}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
