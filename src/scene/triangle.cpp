#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {

Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }

bool Triangle::has_intersection(const Ray &r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.
  Vector3D edge_1 = p2 - p1, edge_2 = p3 - p1, ray_edge = r.o - p1;
  Vector3D s1 = cross(r.d, edge_2), s2 = cross(ray_edge, edge_1);
  Vector3D dots = Vector3D(dot(s2, edge_2), dot(s1, ray_edge), dot(s2, r.d));
  Vector3D fin = 1 / dot(s1, edge_1) * dots;
  double this_t = fin.x, bar1 = fin.y, bar2 = fin.z;
  if(bar1 < 0 || bar2 < 0 || bar1 + bar2 > 1 || this_t < r.min_t || this_t > r.max_t) {
    return false;
  }
  r.max_t = this_t;
  return true;
}

bool Triangle::intersect(const Ray &r, Intersection *isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  Vector3D edge_1 = p2 - p1, edge_2 = p3 - p1, ray_edge = r.o - p1;
  Vector3D s1 = cross(r.d, edge_2), s2 = cross(ray_edge, edge_1);
  Vector3D dots = Vector3D(dot(s2, edge_2), dot(s1, ray_edge), dot(s2, r.d));
  Vector3D fin = 1 / dot(s1, edge_1) * dots;
  double this_t = fin.x, bar1 = fin.y, bar2 = fin.z;
  if(bar1 < 0 || bar2 < 0 || bar1 + bar2 > 1 || this_t < r.min_t || this_t > r.max_t) {
    return false;
  }
  double bar3 = 1.0 - bar1 - bar2;
  r.max_t = this_t;
  isect->n = n1 * bar1 + n2 * bar2 + n3 * bar3;
  isect->t = this_t;
  isect->bsdf = get_bsdf();
  isect->primitive = this;
  return true;
}

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
